using DatabaseIntegration.Entities.Player;
using Services.DTO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Services.Interfaces
{
    public interface ILoginService
    {
        Task<bool> LoginToServer(LoginRequestDTO dto);
        Task<GameCharactersListDTO> SendGameCharactersList(string accountName);
    }
}
