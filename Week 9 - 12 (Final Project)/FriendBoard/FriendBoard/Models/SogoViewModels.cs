using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace FriendBoard.Models
{
    public class SogoViewModels
    {
        public struct SogoIndex
        {
            public string Name;
            public string Image;
            public string Audio;
            public SogoIndex(string name, string imgfilepath, string audiofilepath)
            {
                Name = name;
                Image = imgfilepath;
                Audio = audiofilepath;
            }
        }

        public struct Sogo
        {
            public string Name;
            public string Image;
            public string Audio;
            public int Id;

            public Sogo(string name, string img, string audio, int id)
            {
                Name = name;
                Image = img;
                Audio = audio;
                Id = id;
            }
        }
    }
}