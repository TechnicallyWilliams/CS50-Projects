using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FriendBoard.Models;

namespace FriendBoard.Interfaces
{
    public interface IDisplayIndex
    {
        SogoViewModels.SogoIndex GetIndexSogo(); //Returns a struct aka Object
    }
}
