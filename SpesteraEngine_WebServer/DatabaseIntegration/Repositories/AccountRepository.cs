using DatabaseIntegration.Database;
using DatabaseIntegration.Entities.Player;
using DatabaseIntegration.Interfaces;

namespace DatabaseIntegration.Repositories
{
    public class AccountRepository : IAccountRepository
    {
        private readonly AppDbContext _appDbContext;

        public AccountRepository(AppDbContext context)
        {
            _appDbContext = context;
        }

        public async Task AddNewAccount(GameAccount acc)
        {
            await _appDbContext.Accounts.AddAsync(acc);
            _appDbContext.SaveChanges();
        }
    }
}
