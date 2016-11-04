namespace FriendBoard.Data.Migrations
{
    using System;
    using System.Data.Entity;
    using System.Collections.Generic;
    using System.Data.Entity.Migrations;
    using Microsoft.AspNet.Identity;
    using Microsoft.AspNet.Identity.EntityFramework;
    using FriendBoard.Data.Models;
    using System.Linq;

    internal sealed class Configuration : DbMigrationsConfiguration<FriendBoard.Data.ApplicationDbContext>
    {
        public Configuration()
        {
            AutomaticMigrationsEnabled = false;
        }

        protected override void Seed(ApplicationDbContext db)
        {
            bool seedRoles = true;
            bool seedUsers = true;
            bool Profile = true;
            bool seedImages = true;
            bool seedAudio = true;
            bool LookUp = true;
            bool SocialMedia = true;

            if (seedRoles) CreateRoles(db);
            if (seedUsers) CreateUsers(db);
            if (Profile) CreateProfile(db);
            if (seedImages) CreateImages(db);
            if (seedAudio) CreateAudio(db);
            if (LookUp) CreateImages_Audio(db);
            if (SocialMedia) CreateSocialMedia(db);
        }

        private void CreateRoles(ApplicationDbContext db)
        {
            RoleManager<IdentityRole> roleManager = new RoleManager<IdentityRole>(new RoleStore<IdentityRole>(db));
            if (!roleManager.RoleExists("Admin")) roleManager.Create(new IdentityRole { Name = "Admin" });
            if (!roleManager.RoleExists("User")) roleManager.Create(new IdentityRole { Name = "User" });

        }

        private void CreateUsers(ApplicationDbContext db)
        {
            UserManager<ApplicationUser> userManager = new UserManager<ApplicationUser>(new UserStore<ApplicationUser>(db));
            //inserting; updating; deleting syntax
            //string UserEntry = db.Database.SqlQuery<string>("Select Id from AspNetUsers").FirstOrDefault<string>();

            // String.IsNullOrEmpty(UserEntry))
            //(int) UserEntry[0] == 32 ||
            ApplicationUser User = new ApplicationUser
            {
                UserName = "LukeCage",
                Email = "DexterWilliams04@gmail.com",
                IsActive = true,
                TimesLoggedIn = 1,
                DateCreated = DateTime.UtcNow,
                DateUpdated = DateTime.UtcNow
            };
            userManager.Create(User, "123123");
           // userManager.AddToRole(User.Id, "User"); //This shouldn't be here
            userManager.AddToRole(User.Id, "Admin");
            
        }

        private void CreateProfile(ApplicationDbContext db)
        {
            //inserting; updating; deleting syntax
            //int noOfRowInserted = db.Database.ExecuteSqlCommand(@"CREATE TABLE profile 
            db.Profile.AddOrUpdate(p => p.UserName,
                new Profile
                {    ///didn't know I could just choose a column at the end of this query like that "username"
                    UserName = db.Users.Where(u => u.UserName == "LukeCage").FirstOrDefault().UserName,
                    Avatar = null,
                    AreaCode = 281,
                    Phone = "908-0313",
                    Address = "7756 High Village Dr.",
                    Address2 = "Southcreek Village",
                    City = "Houston",
                    States = "TX",
                    Zip = 77095,
                    UserId = db.Database.SqlQuery<string>("Select Id from dbo.AspNetUsers").FirstOrDefault<string>()
                });

            db.SaveChanges();
        }

        private void CreateImages(ApplicationDbContext db)
        {
            //inserting; updating; deleting syntax
            db.Images.AddOrUpdate(i => i.FileName,
                new Images
                {
                    FileName = "GUILE.png",
                    Name = "GUILE",
                    ProfileId = db.Database.SqlQuery<int>("Select Id from dbo.Profiles").FirstOrDefault<int>() //not working for some weird reason
                });

            db.SaveChanges();
        }

        private void CreateAudio(ApplicationDbContext db)
        {

            db.Audio.AddOrUpdate(a => a.FileName,
                new Audio
                {
                    FileName = "GUILE.wav",
                    ImageId = db.Database.SqlQuery<int>("Select Id from dbo.Images where Id = 1").FirstOrDefault<int>()

                });
        
            db.SaveChanges();
        }

        private void CreateImages_Audio(ApplicationDbContext db)
        {
            db.Images_Audio.AddOrUpdate(i_a => i_a.ImageId,
                new Images_Audio
                {
                    ImageId = db.Database.SqlQuery<int>("Select Id from dbo.Images where Id = 1").FirstOrDefault<int>(),
                    AudioId = db.Database.SqlQuery<int>("Select Id from dbo.Audios where Id = 1").FirstOrDefault<int>(),
                    ProfileId = db.Database.SqlQuery<int>("Select Id from dbo.Profiles").FirstOrDefault<int>()

                });
            db.SaveChanges();
        }

        private void CreateSocialMedia(ApplicationDbContext db)
        {
            //inserting; updating; deleting syntax
            db.SocialMedia.AddOrUpdate(s => s.PublicUrl,
                new SocialMedia
                {
                    IsPublic = true,
                    PublicUrl = "/littleTiger",
                    ProfileId = db.Database.SqlQuery<int>("Select Id from dbo.Profiles").FirstOrDefault<int>()
                });

            db.SaveChanges();

        }

        

    }
}
