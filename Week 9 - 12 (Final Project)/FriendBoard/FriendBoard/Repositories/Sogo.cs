using FriendBoard.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using FriendBoard.Models;
using System.IO;
using FriendBoard.Data;
using System.Data.SqlClient;
using Microsoft.AspNet.Identity;
using Microsoft.AspNet.Identity.EntityFramework;
using System.Data.Entity.Infrastructure;
using FriendBoard.Data.Models;
using System.Text.RegularExpressions;

namespace FriendBoard.Repositories
{
    public class Sogo : IHandleSogos //just learned I can talk to programs in the C drive that don't have a gui by using cmd.exe (ex. iis)
    {
        public void CreateSogo(string sogoName, string imageBytes, string audioBytes)
        {
            //create frontend code that sends in a filled-out object
            if (imageBytes != null && audioBytes != null && sogoName != null)
            {
                using (ApplicationDbContext db = new ApplicationDbContext())
                {
                    //Make sure file doesn't already exist for that user ( I really need unique users or something)
                    //I can append the profileID to the fileName to avoid duplicates in the filedirectory
                    //Then I can just make sure that a user can't use the same name twice 

                    //make sure user doesn't have existing sogoName
                    

                    string userID = HttpContext.Current.User.Identity.GetUserId();

                    int profileID = db.Profile.SqlQuery(@"SELECT *
                     FROM dbo.Profiles
                        WHERE dbo.Profiles.UserId = @p0", userID).FirstOrDefault().Id;


                    string newImageFileName = sogoName + ".png";
                    string newAudioFileName = sogoName + ".wav";

                    string ImageFilePath = "FriendBoard/FriendBoard/Images/" + profileID + newImageFileName;
                    string AudioFilePath = "FriendBoard/FriendBoard/Audio/" + profileID + newAudioFileName;

                    if (File.Exists(ImageFilePath))
                        return;

                    string pattern = @"" + profileID + @"*.png";
                    int fileCount = Directory.GetFiles("FriendBoard/FriendBoard/Images/", pattern).Count();
                    if (fileCount >= 6)
                        return;

                    try
                    {
                        //Create a Sogo
                        db.Database.ExecuteSqlCommand(
                            @"INSERT INTO dbo.Images(FileName, Name, ProfileId)
                        VALUES(@p0, @p1, @p2)", newImageFileName, sogoName, profileID);

                        db.SaveChanges();

                        //This path won't work out on the cloud!!!!!!
                        imageBytes = imageBytes.Replace(' ', '+');
                        File.WriteAllBytes(ImageFilePath, Convert.FromBase64String(imageBytes));


                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                        Console.ReadLine();
                    }

                    try
                    {
                        Images ImageModel = db.Images.SqlQuery("Select * from dbo.Images where Id = (Select MAX(Id) from dbo.Images)").Select(a => new Images
                        {
                            Id = a.Id
                        }).Last(); //can't bind datamodels to themselves? yes you can. Its that you forgot that you can step through last to get a column .Last().Id

                        //Create a Sogo
                        db.Database.ExecuteSqlCommand(
                            @"INSERT INTO dbo.Audios(FileName, ImageId)
                                VALUES(@p0, @p1)", newAudioFileName, ImageModel.Id);

                        db.SaveChanges();

                        //This path won't work out on the cloud!!!!
                        audioBytes = audioBytes.Replace(' ', '+');
                        File.WriteAllBytes(AudioFilePath, Convert.FromBase64String(audioBytes));

                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                        Console.ReadLine();
                    }
                  
                }
            }
        }

        public List<SogoViewModels.Sogo> GetSogos(int id)
        {
            using (ApplicationDbContext db = new ApplicationDbContext())
            {
                List<SogoViewModels.Sogo> sogoList;

                string userID = HttpContext.Current.User.Identity.GetUserId();
                int profileID = db.Profile.SqlQuery("SELECT * FROM dbo.Profiles WHERE dbo.Profiles.UserId = @p0", userID).FirstOrDefault().Id; //Need to select all columns when using a data entity

                // query without relying on virtual and view model
                // List<SogoViewModels.SogoIndex> IndexSogo = new List<SogoViewModels.SogoIndex>();
                /* sogoList = db.Database.SqlQuery<SogoViewModels.Sogo>(@"SELECT dbo.Audios.FileName, dbo.Images.FileName, dbo.Images.Name
                     FROM dbo.Audios
                         INNER JOIN dbo.Images
                             ON dbo.Audios.ImageId = dbo.Images.Id 
                                WHERE dbo.Images.ProfileId = @p0;
                 ", profileID).ToList(); //This yield one result when it should have yielded more (maybe because the where or because nullable int)
                 */

                //https://msdn.microsoft.com/en-us/library/h32h3abf(v=vs.110).aspx
                //A good database structure will make these queries more resistant to refactoring
                sogoList = db.Audio.SqlQuery(@"SELECT *
                     FROM dbo.Audios
                         INNER JOIN dbo.Images
                             ON dbo.Audios.ImageId = dbo.Images.Id
                                WHERE dbo.Images.ProfileId = @p0", profileID).Select(a => new SogoViewModels.Sogo
                {
                    Audio = profileID + a.FileName,
                    Image = profileID + a.Images.FileName, //Maybe stepping
                    Name = a.Images.Name,
                    Id = a.Id
                }).ToList();

                
                //db.Database.Connection.Close();
                //db.Dispose();

                //It probably wasn't the data bind, it was probably because nullable int is different than int
                return sogoList;
            }
        }

        public void UpdateSogo(string sogoName, string imageBytes, string audioBytes, int audioId)
        {
            ApplicationDbContext db = new ApplicationDbContext();

            //File io projects need to be C:\Program Files (x86)\IIS Express\

            /*  string startupPath = System.IO.Directory.GetCurrentDirectory();
              string startupPath2 = Environment.CurrentDirectory;
              int hmm = startupPath.Length;
              string more = Directory.GetParent(Directory.GetCurrentDirectory()).Parent.FullName;
              string testPath = System.IO.Path.GetFullPath(@"..\..\");
              string winDir = System.Environment.GetEnvironmentVariable("FriendBoard");
              string[] dirs = Directory.GetDirectories(winDir);

              //change then name of both existing files then update database
              string[] files = Directory.GetFiles(@"\\Audio\", "*.wav", SearchOption.AllDirectories);
              var filelist = new DirectoryInfo(@"\\Audio\").GetFiles("*.wav", SearchOption.AllDirectories); */

            SogoViewModels.Sogo sogo = new SogoViewModels.Sogo();

            //get profileID and Name so you know the name of the current file
            //db types
            //how do i find existing file name a: by going to the database first
            sogo = db.Images.SqlQuery(@"SELECT *
                     FROM dbo.Images
                         INNER JOIN dbo.Audios
                             ON dbo.Images.Id = dbo.Audios.ImageId
                                WHERE dbo.Audios.Id = @p0", audioId).Select(d => new SogoViewModels.Sogo
            {
                Name = d.Name, //ProfileId, Name, FileName
                Id = d.ProfileId
            }).First();

            string oldAudioFileName = "FriendBoard/FriendBoard/Audio/" + sogo.Id + sogo.Name + ".wav";
            string oldImageFileName = "FriendBoard/FriendBoard/Images/" + sogo.Id + sogo.Name + ".jpg";

            //function();

            //my javascript front will never send a sogo with all three parameters so it's best to just set up if-statements
            if (sogoName != null)
            {
                string newImageFileName = sogoName + ".png";
                string newAudioFileName = sogoName + ".wav";

                db.Database.ExecuteSqlCommand(@"UPDATE Images
                SET FileName = @p0, Name = @p1
                    WHERE Id = @p2", newImageFileName, sogoName, audioId);

                db.Database.ExecuteSqlCommand(@"UPDATE Audios
                SET FileName = @p0
                    WHERE Id = @p1", newAudioFileName, audioId);

                db.SaveChanges();

                newImageFileName = sogo.Id + newImageFileName;
                newAudioFileName = sogo.Id + newAudioFileName;

                File.Move(oldAudioFileName, newAudioFileName);
                File.Move(oldImageFileName, newImageFileName);

            }
            else if (imageBytes != null) //Don't need to talk to database
            {
                //remove existing file and create new or update existing (simply replace byte content)
                //We need more customization, right now, I'll override existing names with null by accident
                string filePath = oldImageFileName;
                imageBytes = imageBytes.Replace(' ', '+');

                File.WriteAllBytes(filePath, Convert.FromBase64String(imageBytes));
            }
            else //Don't need to talk to database
            {
                File.Delete(oldAudioFileName);
                string filePath = oldAudioFileName;
                audioBytes = audioBytes.Replace(' ', '+');
                File.WriteAllBytes(filePath, Convert.FromBase64String(audioBytes));
            }

        }

        public void DeleteSogo(int audioId)
        {
            ApplicationDbContext db = new ApplicationDbContext();
            SogoViewModels.Sogo sogo = new SogoViewModels.Sogo();
           

            sogo = db.Images.SqlQuery(@"SELECT *
                     FROM dbo.Images
                         INNER JOIN dbo.Audios
                             ON dbo.Images.Id = dbo.Audios.ImageId
                                WHERE dbo.Audios.Id = @p0", audioId).Select(d => new SogoViewModels.Sogo
            {
                Name = d.Name, //ProfileId, Name, FileName
                Id = d.ProfileId
            }).First();

           

            string oldAudioFileName = "FriendBoard/FriendBoard/Audio/" + sogo.Id + sogo.Name + ".wav";
            string oldImageFileName = "FriendBoard/FriendBoard/Images/" + sogo.Id + sogo.Name + ".png";

            db.Database.ExecuteSqlCommand(@"DELETE FROM dbo.Audios
                    WHERE Id = @p0", audioId);

            db.SaveChanges();

            db.Database.ExecuteSqlCommand(@"DELETE FROM dbo.Images
                    WHERE Id = @p0", audioId); //this would be better if you used imageId instead

            db.SaveChanges();

            //It worked, it actually deleted it
            File.Delete(oldAudioFileName);
            File.Delete(oldImageFileName);
        }
    }
}