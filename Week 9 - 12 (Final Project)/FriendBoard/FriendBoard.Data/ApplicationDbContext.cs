using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.Entity;
using Microsoft.AspNet.Identity.EntityFramework;
using FriendBoard.Data.Models;

namespace FriendBoard.Data
{
    public class ApplicationDbContext : IdentityDbContext<ApplicationUser>
    {
        public DbSet<Profile> Profile { get; set; }
        public DbSet<SocialMedia> SocialMedia { get; set; }
        public DbSet<Audio> Audio { get; set; }
        public DbSet<Images> Images { get; set; }
        public DbSet<Images_Audio> Images_Audio { get; set; }

        public ApplicationDbContext()
            : base("DefaultConnection", throwIfV1Schema: false)
        {
        }

        public static ApplicationDbContext Create()
        {
            return new ApplicationDbContext();
        }
    }
}
