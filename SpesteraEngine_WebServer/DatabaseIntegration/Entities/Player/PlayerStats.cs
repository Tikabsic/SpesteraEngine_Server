using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseIntegration.Entities.Player
{
    public class PlayerStats
    {
        [Key]
        public Guid GameCharacterId { get; set; }
        public GameCharacter GameCharacter { get; set; }
        public int MaxHp { get; set; }
        public int CurrentHp { get; set; }
        public int MaxMana { get; set; }
        public int CurrentMana { get; set; }
        public int MaxStamina { get; set; }
        public int CurrentStamina { get; set; }
        public int CharacterLevelExperiencePoints { get; set; }
    }
}
