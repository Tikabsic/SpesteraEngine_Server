using DatabaseIntegration.Entities.Player;

namespace Services.DTO
{
    public class LoginResponseDTO
    {
        public string JWTToken { get; set; }
        public List<GameCharacter> characters { get; set; }
    }
}
