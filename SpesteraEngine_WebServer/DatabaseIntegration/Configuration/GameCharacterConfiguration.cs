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
    internal class GameCharacterConfiguration : IEntityTypeConfiguration<GameCharacter>
    {
        public void Configure(EntityTypeBuilder<GameCharacter> builder)
        {
            builder.HasKey(gc => gc.CharacterId);

            builder.HasIndex(gc => gc.Nickname).IsUnique();

            builder.HasIndex(gc => gc.AccountId).IsUnique();

            builder.Property(gc => gc.Nickname).HasMaxLength(18);


            //builder.HasOne(gc => gc.PlayerStats)
            //    .WithOne(ps => ps.GameCharacter);

            //builder.HasOne(gc => gc.PlayerSkills)
            //    .WithOne(ps => ps.GameCharacter);

            //builder.HasOne(gc => gc.Transform)
            //    .WithOne(t => t.GameCharacter);

            //builder.HasMany(gc => gc.Inventory)
            //    .WithOne(oi => oi.Owner)
            //    .HasForeignKey(oi => oi.OwnerId);
        }
    }
}
