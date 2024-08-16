using Services.DTO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Services.Interfaces
{
    public interface IRegisterService
    {
        Task<string> RegisterNewUser(RegisterRequestDTO dto);
    }
}
