using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace FriendBoard.Models
{
    public class RegisterViewModel
    {
        public string UserName { get; set; }
        public string Password { get; set; }
        public string Avatar { get; set; }
        public string Email { get; set; }
        public int AreaCode { get; set; }
        public string Phone { get; set; }

        public string Address { get; set; }

        public string Address2 { get; set; }

        public string City { get; set; }
        public string States { get; set; }

        public int Zip { get; set; }

    }
}