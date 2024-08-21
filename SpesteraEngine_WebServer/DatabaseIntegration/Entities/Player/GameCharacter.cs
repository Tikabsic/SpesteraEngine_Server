using DatabaseIntegration.Entities.Items;

namespace DatabaseIntegration.Entities.Player
{
    public class GameCharacter
    {
        public GameAccount Account { get; set; }
        public string AccountId { get; set; }
        public string CharacterId { get; set; }
        public int SlotNumber { get; set; }
        public string Nickname { get; set; }
        public float PositionX { get; set; }
        public float PositionY { get; set; }
        public float PositionZ { get; set; }
    }
}
