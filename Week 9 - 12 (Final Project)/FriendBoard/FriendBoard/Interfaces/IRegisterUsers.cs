using FriendBoard.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FriendBoard.Interfaces
{
    interface IRegisterUsers
    {
        void CreateUser(RegisterViewModel model);
    }
}
