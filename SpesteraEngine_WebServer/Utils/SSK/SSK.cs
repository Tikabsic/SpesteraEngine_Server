using System.Security.Cryptography;
using System.Text;

namespace Utils.SSK
{
    public class SSKey
    {
        private readonly byte[] _securityCode = Convert.FromBase64String("pGf+lh2QOloKXU/mVh5k4ErP76mkcG6Dt1TZMZTj4Ks=");

        public SSKey()
        {
            if (_securityCode.Length != 32)
            {
                throw new ArgumentException("Security code must be 32 bytes (256 bits) long.");
            }
        }

        public string EncryptSSK(string message)
        {
            using (var aes = Aes.Create())
            {
                aes.Key = _securityCode;
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;
                aes.GenerateIV();
                using (var encryptor = aes.CreateEncryptor(aes.Key, aes.IV))
                {
                    byte[] plainText = Encoding.UTF8.GetBytes(message);
                    byte[] encrypted = encryptor.TransformFinalBlock(plainText, 0, plainText.Length);

                    byte[] result = new byte[aes.IV.Length + encrypted.Length];
                    Buffer.BlockCopy(aes.IV, 0, result, 0, aes.IV.Length);
                    Buffer.BlockCopy(encrypted, 0, result, aes.IV.Length, encrypted.Length);

                    return Convert.ToBase64String(result);
                }
            }
        }

        public string DecryptSSK(string message)
        {
            byte[] encryptedData = Convert.FromBase64String(message);
            using (var aes = Aes.Create())
            {
                aes.Key = _securityCode;
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;

                byte[] iv = new byte[aes.BlockSize / 8];
                Buffer.BlockCopy(encryptedData, 0, iv, 0, iv.Length);
                aes.IV = iv;

                using (var decryptor = aes.CreateDecryptor(aes.Key, aes.IV))
                {
                    byte[] decrypted = decryptor.TransformFinalBlock(encryptedData, iv.Length, encryptedData.Length - iv.Length);
                    return Encoding.UTF8.GetString(decrypted);
                }
            }
        }
    }
}
