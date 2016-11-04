using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace FriendBoard.Data.Models
{
    public class Audio
    {
        [Key]
        public int Id { get; set; }
        public string FileName { get; set; }
        public int ImageId { get; set; }
        [ForeignKey("ImageId")]
        public virtual Images Images { get; set; }
    }
}
