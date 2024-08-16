
using System.ComponentModel.DataAnnotations;

namespace DatabaseIntegration.Entities.Player
{
    public class PlayerSkills
    {
        [Key]
        public Guid GameCharacterId { get; set; }
        public GameCharacter GameCharacter { get; set; }
        public int Strength { get; set; }
        public int Agility { get; set; }
        public int Defence { get; set; }
    }
}
