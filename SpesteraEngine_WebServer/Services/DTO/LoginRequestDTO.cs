using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Services.DTO
{
    public class LoginRequestDTO
    {
        public string AccountName { get; set; }
        public string Password { get; set; }
    }
}
