using DatabaseIntegration.Database;
using DatabaseIntegration.Interfaces;
using Services.DTO;
using Services.Interfaces;
using Services.Validator;
using Utils.Hasher;

namespace Services.AccountServices
{
    internal class RegisterService : IRegisterService
    {
        private readonly IAccountRepository _accountRepository;
        private readonly AppDbContext _dbContext;
        private readonly IPasswordHasher _passwordHasher;
        public RegisterService(IAccountRepository accountRepository,
                                AppDbContext context,
                               IPasswordHasher passwordHasher)

        {
            _accountRepository = accountRepository;
            _dbContext = context;
            _passwordHasher = passwordHasher;
        }

        public async Task<string> RegisterNewUser(RegisterRequestDTO dto)
        {
            var validator = new AccountRegistrationValidator(_dbContext);
            var validationresult = validator.Validate(dto);

            if (!validationresult.IsValid)
            {
                throw new Exception("Validation failed: " + string.Join(", ", validationresult.Errors.Select(e => e.ErrorMessage)));
            }

            var hashedPassword = _passwordHasher.Hash(dto.Password);

            DatabaseIntegration.Entities.Player.GameAccount newAccount = new DatabaseIntegration.Entities.Player.GameAccount();

            newAccount.Email = dto.Email;
            newAccount.AccountName = dto.AccountName;
            newAccount.Password = hashedPassword;
            newAccount.Nationality = DatabaseIntegration.Enums.Nationality.Polish;
            newAccount.CreationDate = DateTime.Now;

            await _accountRepository.AddNewAccount(newAccount);

            return "Register succeeded.";
        }
    }
}
