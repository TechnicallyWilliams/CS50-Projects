using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace FriendBoard.Data.Models
{
    public class Images
    {
        [Key]
        public int Id { get; set; }

        //This shoudl have a name property
        public string FileName { get; set; }
        public string Name { get; set; }

        public int ProfileId { get; set; }
        [ForeignKey("ProfileId")]
        public virtual Profile Profile { get; set; }
    }
}
