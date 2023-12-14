#ifndef EZ_WAV_h
#define EZ_WAV_h

#include <inttypes.h>
#include <stdio.h>
#include <malloc.h>

enum AUDIO_FMTS {
    FMT_PCM = 1
};

//#define STD_CHUNKID_        0x52494646 
//#define STD_FMT_            0x57415645 
//#define STD_SUBCHUNK1ID_    0x666D7420 
//#define STD_SUBCHUNK2ID_    0x64617461 


#define STD_CHUNKID_        0x46464952
#define STD_FMT_            0x45564157
#define STD_SUBCHUNK1ID_    0x20746D66
#define STD_SUBCHUNK2ID_    0x61746164


// Data is little endian unless noted
struct WAVE_Header {
    // RIFF Chunk Descriptor
    uint32_t    ChunkID;        // 'RIFF' Big Endian
    uint32_t    ChunkSize;      // 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
    uint32_t    Format;         // 'WAVE' Big Endian
    
    // Format Sub-Chunk
    uint32_t    Subchunk1ID;    // 'fmt ' Big Endian
    uint32_t    Subchunk1Size;  // 16 for PCM
    uint16_t    AudioFormat;    // 1 == PCM
    uint16_t    NumChannels;    // Well... Yeah
    uint32_t    SampleRate;     // ...
    uint32_t    ByteRate;       // SampleRate * NumChannels * BitsPerSample / 8
    uint16_t    BlockAlign;     // NumChannels * BitsPerSample/8
    uint16_t    BitsPerSample;

    // Data Sub-Chunk
    uint32_t    Subchunk2ID;    // 'Data' Big Endian
    uint32_t    Subchunk2Size;  // NumSamples * NumChannels * BitsPerSample/8
    void        *data;
};


struct WAVE_Data {
    uint16_t    num_channels;
    uint16_t    dlen;
    uint16_t    dsize;
    void        **data_arr;
};


void print_wave_header(struct WAVE_Header *wav){
    if(wav){
        printf("Chunk ID:\t%c%c%c%c\t(0x%08X)\n", 
                                        *((char *)&wav->ChunkID),
                                        *((char *)&wav->ChunkID + 1u),
                                        *((char *)&wav->ChunkID + 2u),
                                        *((char *)&wav->ChunkID + 3u),
                                        wav->ChunkID);
        printf("Chunk Size:\t%lu bytes\n", wav->ChunkSize);
        printf("Format:\t\t%c%c%c%c\t(0x%08X)\n", 
                                        *((char *)&wav->Format),
                                        *((char *)&wav->Format + 1u),
                                        *((char *)&wav->Format + 2u),
                                        *((char *)&wav->Format + 3u), 
                                        wav->Format);
        printf("Audio Format:\t%s\n", (wav->AudioFormat == FMT_PCM) ? "PCM" : "???");
        printf("Channels:\t%u\n", wav->NumChannels);
        printf("Sample Rate:\t%lu Hz\n", wav->SampleRate);
        printf("Byte Rate:\t%lu bytes/sec\n", wav->ByteRate);
        printf("Bits/Sample:\t%u\n", wav->BitsPerSample);

        printf("Data Size:\t%lu bytes\n", wav->Subchunk2Size);

        uint32_t num_samples = wav->Subchunk2Size / wav->BitsPerSample;
        uint32_t samples_per_chan = num_samples / wav->NumChannels;

        printf("Num Samples:\t%lu\n", num_samples);
        printf("Samples/Chan:\t%lu\n", samples_per_chan);
        
        
    }
}

// Assumption made that L R L R interleaving is done for 2 channels
// Dsize is bits, dlen is number of datatype_len entries. u16 -> dsize = 16, dlen = 1
int output_wav(struct WAVE_Header *wav, char *fname, void *data, uint16_t dsize, uint32_t dlen, uint32_t fs, uint16_t channels){
    if((dsize != 8) && (dsize != 16)) return -1;
    if(!data || channels > 2) return -2;

    wav->ChunkID        = STD_CHUNKID_;
    wav->Format         = STD_FMT_;
    wav->Subchunk1ID    = STD_SUBCHUNK1ID_;
    wav->Subchunk2ID    = STD_SUBCHUNK2ID_;

    wav->Subchunk1Size  = 16;

    wav->AudioFormat    = FMT_PCM;

    wav->NumChannels    = channels;
    wav->SampleRate     = fs;
    wav->ByteRate       = fs * channels * (dsize >> 3);
    wav->BlockAlign     = channels * (dsize >> 3);
    wav->BitsPerSample  = dsize;

    wav->Subchunk2Size  = channels * dlen * (dsize >> 3);
    wav->data           = data;
    
    wav->ChunkSize      = 4 + (8 + wav->Subchunk1Size) + (8 + wav->Subchunk2Size);

    FILE *fp;
    fp = fopen(fname, "wb");

    fwrite(wav, (sizeof(struct WAVE_Header) - sizeof(void *)), 1, fp);

    fwrite(data, dlen, (dsize >> 3), fp);

    fclose(fp);

    return 0;
}


int read_wav(struct WAVE_Header *wav, char *fname){
    if(!wav || !fname) return -2;
    FILE *fp;
    fp = fopen(fname, "rb");
    fread(wav, (sizeof(struct WAVE_Header) - sizeof(void *)), 1, fp);

    

    fclose(fp);
}

int free_read_data(struct WAVE_Header *wav){
    free(wav->data);
}


#endif