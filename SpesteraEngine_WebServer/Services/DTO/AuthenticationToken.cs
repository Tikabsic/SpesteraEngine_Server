using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace Services.DTO
{
    public class AuthenticationToken
    {
        public Guid Token { get; set; }
        public Guid GameCharacterId { get; set; }
        public IPAddress IPAddress { get; set; }
        public int NetworkID { get; set; }
    }
}
