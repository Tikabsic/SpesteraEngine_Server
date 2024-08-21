using DatabaseIntegration.Database;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;
using Services.Authentication;
using Services.DTO;
using Services.Interfaces;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Text;
using Utils.Hasher;
using Utils.SSK;

namespace Services.Account
{
    internal class LoginService : ILoginService
    {
        private readonly AppDbContext _dbContext;
        private readonly IPasswordHasher _passwordHasher;
        private readonly AuthenticationSettings _authenticationSettings;

        public LoginService(AppDbContext context,
            IPasswordHasher hasher,
            AuthenticationSettings autenticationSettings)
        {
            _dbContext = context;
            _passwordHasher = hasher;
            _authenticationSettings = autenticationSettings;
        }

        public async Task<bool> LoginToServer(LoginRequestDTO dto)
        {
            var account = await _dbContext.Accounts.FirstOrDefaultAsync(a => a.AccountName == dto.AccountName);

            if (account == null)
            {
                throw new Exception("Account not found");
            }

            var passwordResult = _passwordHasher.Verify(account.Password, dto.Password);

            if (!passwordResult)
            {
                throw new Exception("Wrong login or password");
            }
            return true;

        }

        public async Task<string> GenerateJWTToken(string accountName)
        {
            var user = await _dbContext.Accounts.FirstOrDefaultAsync(u => u.AccountName == accountName);

            var claims = new List<Claim>()
            {
                new Claim("Id", user.AccountId.ToString()),
                new Claim("Name", user.AccountName.ToString()),
            };

            var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_authenticationSettings.JWTKey));
            var credentials = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);
            var expires = DateTime.Now.AddDays(_authenticationSettings.JWTExpireDays);
            var token = new JwtSecurityToken(_authenticationSettings.JWTIssuer, _authenticationSettings.JWTIssuer, claims, expires: expires, signingCredentials: credentials);
            var handler = new JwtSecurityTokenHandler();

            return handler.WriteToken(token).ToString();
        }

        public async Task<LoginResponseDTO> SendLoginData(string accountName)
        {
            var gameCharacters = await _dbContext.GameCharacters
                .AsNoTracking()
                .Where(g => g.Account.AccountName == accountName)
                .ToListAsync();
            SSKey encryptor = new SSKey();
            var LoginResponseDTO = new LoginResponseDTO
            {
                JWTToken = encryptor.EncryptSSK(await GenerateJWTToken(accountName)),
                characters = gameCharacters
            };

            return LoginResponseDTO;
        }
    }
}
