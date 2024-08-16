//using DatabaseIntegration.Entities.Player;
//using Microsoft.EntityFrameworkCore;
//using Microsoft.EntityFrameworkCore.Metadata.Builders;
//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;

//namespace DatabaseIntegration.Configuration
//{
//    internal class PlayerStatsConfiguration : IEntityTypeConfiguration<PlayerStats>
//    {
//        public void Configure(EntityTypeBuilder<PlayerStats> builder)
//        {
//            builder.HasIndex(ps => ps.GameCharacterId).IsUnique();          
//        }
//    }
//}
