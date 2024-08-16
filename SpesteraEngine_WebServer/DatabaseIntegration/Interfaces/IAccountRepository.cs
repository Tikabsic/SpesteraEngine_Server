
using DatabaseIntegration.Entities.Player;

namespace DatabaseIntegration.Interfaces
{
    public interface IAccountRepository
    {
        Task AddNewAccount(GameAccount acc);
    }
}
