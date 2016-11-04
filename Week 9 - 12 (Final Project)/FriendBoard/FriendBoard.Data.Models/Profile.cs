using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace FriendBoard.Data.Models
{
    public class Profile
    {
        [Key]
        public int Id { get; set; }
        public string UserName { get; set; }
        public string Avatar { get; set; }
        public int AreaCode { get; set; }
        public string Phone { get; set; }

        public string Address { get; set; }

        public string Address2 { get; set; }

        public string City { get; set; }
        public string States { get; set; }
        
        public int Zip { get; set; }

        public string UserId { get; set; }
        [ForeignKey("UserId")]
        public virtual ApplicationUser ApplicationUser { get; set; }

    }
}
