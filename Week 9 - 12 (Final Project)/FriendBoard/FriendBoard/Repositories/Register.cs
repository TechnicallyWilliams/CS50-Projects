using FriendBoard.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using FriendBoard.Models;
using FriendBoard.Data;
using Microsoft.AspNet.Identity.EntityFramework;
using FriendBoard.Data.Models;
using Microsoft.AspNet.Identity;

namespace FriendBoard.Repositories
{
    public class Register : IRegisterUsers
    {
        public void CreateUser(RegisterViewModel data)
        {
            ApplicationDbContext db = new ApplicationDbContext(); //below, ef and identity work togeth
            UserStore<ApplicationUser> store = new UserStore<ApplicationUser>(db); //asp.net's Entity framework has a store
            UserManager<ApplicationUser> userManager = new UserManager<ApplicationUser>(store); //asp.net's identity has a usermanager

            if (!db.Users.Any(u => u.UserName == data.UserName || u.Email == data.Email))
            {
                ApplicationUser User = new ApplicationUser { UserName = data.UserName, Email = data.Email };
                userManager.Create(User, data.Password);
                userManager.AddToRole(User.Id, "User");

                string UserId = db.Users.Where(u => u.Email == data.Email).FirstOrDefault().Id;

                //Add with a sql statement instead
                db.Database.ExecuteSqlCommand(
                        @"INSERT INTO dbo.Profiles(UserName, Avatar, AreaCode, Phone, Address, Address2, City, States, Zip, UserId)
                                VALUES(@p0, @p1, @p2, @p3, @p4, @p5, @p6, @p7, @p8, @p9)", data.UserName, data.Avatar, data.AreaCode, data.Phone, data.Address, data.Address2, data.City, data.States, data.Zip, UserId);

                db.SaveChanges();
            }
        }
    }
}