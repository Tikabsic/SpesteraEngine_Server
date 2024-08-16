using DatabaseIntegration.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Services.DTO
{
    public class RegisterRequestDTO
    {
        public string Email { get; set; }
        public string AccountName { get; set; }
        public string Password { get; set; }
        public string ConfirmationPassword { get; set; }
    }
}
