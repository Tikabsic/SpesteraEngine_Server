using DatabaseIntegration.Entities.Player;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseIntegration.Configuration
{
    internal class AccountConfiguration : IEntityTypeConfiguration<GameAccount>
    {
        public void Configure(EntityTypeBuilder<GameAccount> builder)
        {
            builder.HasKey(a => a.AccountId);

            builder.HasIndex(a => a.AccountName).IsUnique();
            builder.HasIndex(a => a.AccountId).IsUnique();

            builder.HasMany(a => a.GameCharacters)
                .WithOne(gc => gc.Account)
                .HasForeignKey(gc => gc.AccountId);

            builder.Property(a => a.AccountName).HasMaxLength(18);
            builder.Property(a => a.Email).HasMaxLength(30);
        }
    }
}
