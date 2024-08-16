using DatabaseIntegration.Enums;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseIntegration.Entities.Player
{
    public class GameAccount
    {
        public Guid Id { get; set; }
        public DateTime CreationDate { get; set; }
        public Nationality Nationality { get; set; }
        public string Email { get; set; }
        public string AccountName { get; set; }
        public string Password { get; set; }
        public List<GameCharacter> GameCharacters { get; set; }

    }
}
