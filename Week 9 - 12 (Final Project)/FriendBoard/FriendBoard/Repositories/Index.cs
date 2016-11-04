using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using FriendBoard.Models;
using FriendBoard.Data;
using FriendBoard.Interfaces;
using System.Data.SqlClient;
using Microsoft.AspNet.Identity;
using Microsoft.AspNet.Identity.EntityFramework;
using System.Data.Entity.Infrastructure;
using FriendBoard.Data.Models;

namespace FriendBoard.Repositories
{
    public class Index : IDisplayIndex
    {
        public SogoViewModels.SogoIndex GetIndexSogo()
        {
            ApplicationDbContext db = new ApplicationDbContext();

            //SqlConnection connection = new SqlConnection("Data Source = (LocalDb);Initial Catalog = aspnet - FriendBoard - 20161005045635; Integrated Security = True");

            //SqlCommand cmd = new SqlCommand("SELECT");

            //inserting; updating; deleting syntax
            var TotalList = db.Images_Audio.SqlQuery("Select * from dbo.Images_Audio").ToList().Count();
                  
            if (TotalList != 0 && TotalList != 1)
            {
                int NewIndex = new Random().Next(1, TotalList + 1); //Give it ability to choose highest number

                if (NewIndex == TotalList + 1)
                {
                    NewIndex--;
                }
            } //else show default photo


            //parameterized sql query
            //using the entity type, it requires me to ask for all column names
            var logo = db.Images.SqlQuery("Select FileName, Id, Name, ProfileId from dbo.Images where Id=@p0", TotalList).Single();

            //sogo.FileName;

            /* foreach(object field in Logo)
             {
                 object check = field;
             } */

            //db.Database.SqlQuery<int>("Select Id from dbo.Images").FirstOrDefault<int>();

            var sound = db.Audio.SqlQuery("Select FileName, Id, ImageId from dbo.Audios where Id = @p0", logo.Id).Single();

            // String.IsNullOrEmpty(UserEntry))
            //(int) UserEntry[0] == 32 ||
            //<script src="~/FrontEnd/Images/IndexController.js"></script>
            string ImageName = logo.Id + logo.FileName; //Not putting the file path in the database is a great idea if solution directory ever needs changes.
            string AudioName = logo.Id + sound.FileName;

             SogoViewModels.SogoIndex IndexSogo = new SogoViewModels.SogoIndex(logo.Name, ImageName, AudioName);

            db.Database.Connection.Close();
            //db.Dispose(); Testing to see if this removes the index errors on the site


            return IndexSogo;

        }
    }
}