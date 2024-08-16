using DatabaseIntegration.Entities.Player;
using System.ComponentModel.DataAnnotations;

namespace DatabaseIntegration.Entities.Items
{
    public class ObtainableItem
    {
        public GameCharacter Owner { get; set; }
        [Key]
        public Guid OwnerId { get; set; }
        public int ItemId { get; set; }
        public string ItemName { get; set; }
        public int Quantity { get; set; }
        public int Quality { get; set; }
        public int ItemIndex { get; set; }
    }
}
