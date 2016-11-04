using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace FriendBoard.Data.Models
{
    public class Images_Audio
    {
        [Key]
        public int Id { get; set; }
        public int ImageId { get; set; }
        public int AudioId { get; set; }
        public int ProfileId { get; set; }
        [ForeignKey("ProfileId")]
        public virtual Profile Profile { get; set; }
    }

}
