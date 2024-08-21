using DatabaseIntegration.Database;
using DatabaseIntegration.Entities.Player;
using DatabaseIntegration.Interfaces;
using Microsoft.EntityFrameworkCore;

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

        public async Task<bool> CheckId(string accountId)
        {
            return await _appDbContext.Accounts.AnyAsync(a => a.AccountId == accountId);
        }
    }
}
