using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Services.Authentication
{
    public class AuthenticationSettings
    {
        public string JWTKey { get; set; }
        public int JWTExpireDays { get; set; }
        public string JWTIssuer { get; set; }
    }
}
