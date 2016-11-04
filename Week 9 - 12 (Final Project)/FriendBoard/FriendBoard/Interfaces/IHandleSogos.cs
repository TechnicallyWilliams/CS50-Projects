using FriendBoard.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FriendBoard.Interfaces
{
    interface IHandleSogos
    {
        List<SogoViewModels.Sogo> GetSogos(int id); //Returns a struct aka Object

        void CreateSogo(string sogoName, string imageBytes, string audioBytes);

        void UpdateSogo(string sogoName, string imageBytes, string audioBytes, int audioId);

        void DeleteSogo(int audioId);

    }
}
