#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

namespace little_endian_io //создание пространства имён с обратным порядком следования байтов
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8) //
      outs.put( static_cast <char> (value & 0xFF) ); // приведение к типу char, 0xFF - это максимальное значение 1 байта
    return outs;
  }
}
using namespace little_endian_io;

int main()
{
  ofstream f( "D:/tddProject/IDZ1/IDZ1_WAV_generation/Test2.wav", ios::binary );

  // Создание заголовочной части WAV файла
  f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
  write_word( f,     16, 4 );  // no extension data
  write_word( f,      1, 2 );  // PCM - integer samples
  write_word( f,      2, 2 );  // two channels (stereo file)
  write_word( f,  44100, 4 );  // samples per second (Hz)
  write_word( f, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
  write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
  write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

  // Write the data chunk header
  size_t data_chunk_pos = f.tellp();
  f << "data----";  // (chunk size to be filled in later)

  // Write the audio samples
  constexpr double two_pi = 6.283185307179586476925286766559;
  constexpr double amplitude_1 = 32760;  //амплитуда однотонального сигнала

  double hz        = 44100;    // частота дискретизации
  double frequency = 261.626;  // тон генерируемого сигнала
  double seconds   = 5;      // длительность воспроизведения

  int N = hz * seconds;  // число отсчётов
  for (int n = 0; n < N; n++)
  {
    double amplitude_2 = amplitude_1 +(double)(rand()%3000);
    double value     = sin( (two_pi * n * frequency) / hz );
    write_word( f, (int)(                 amplitude_2  * value), 2 );
    write_word( f, (int)((amplitude_2) * value), 2 );
  }

  size_t file_length = f.tellp(); // позиция текущего отсчёта в выходном потоке

  // Fix the data chunk header to contain the data size
  f.seekp( data_chunk_pos + 4 ); // смещение файлового указателя в выходном потоке
  write_word( f, file_length - data_chunk_pos + 8 );

  // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
  f.seekp( 0 + 4 ); // смещение файлового указателя в выходном потоке
  write_word( f, file_length - 8, 4 );
}
