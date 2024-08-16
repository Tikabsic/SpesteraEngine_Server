using DatabaseIntegration.Entities.Items;
using DatabaseIntegration.Entities.Player;
using Microsoft.EntityFrameworkCore;

namespace DatabaseIntegration.Database
{
    public class AppDbContext : DbContext
    {
        public DbSet<GameAccount> Accounts { get; set; }
        public DbSet<GameCharacter> GameCharacters { get; set; }
        //public DbSet<PlayerStats> PlayersStats { get; set; }
        //public DbSet<PlayerSkills> PlayersSkills { get; set; }
        //public DbSet<ObtainableItem> ObtainableItems { get; set; }

        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options)
        {

        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)

        {
            modelBuilder.ApplyConfigurationsFromAssembly(typeof(AppDbContext).Assembly);
        }

    }
}
