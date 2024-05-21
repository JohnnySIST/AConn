#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opus.h>
#include "config.hpp"

#define FRAME_SIZE 960
#define MAX_PACKET_SIZE (3 * 1276)

int main() {
    OpusEncoder *encoder;
    OpusDecoder *decoder;
    int error;

    /* Initialize encoder */
    encoder = opus_encoder_create(SAMPLE_RATE, NUM_CHANNELS, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK) {
        fprintf(stderr, "Failed to initialize encoder: %s\n", opus_strerror(error));
        return EXIT_FAILURE;
    }

    /* Initialize decoder */
    decoder = opus_decoder_create(SAMPLE_RATE, NUM_CHANNELS, &error);
    if (error != OPUS_OK) {
        fprintf(stderr, "Failed to initialize decoder: %s\n", opus_strerror(error));
        opus_encoder_destroy(encoder);
        return EXIT_FAILURE;
    }

    /* Read audio data from file or input stream */
    /* For simplicity, we are generating a sine wave as input */
    const float frequency = 440.0;  // A4
    const short amplitude = 0.3 * 32767;
    const float duration = 3.0;
    const int num_samples = SAMPLE_RATE * duration;
    short *input_samples = (short *)malloc(sizeof(short) * num_samples);
    for (int i = 0; i < num_samples; ++i) {
        float t = (float)i / SAMPLE_RATE;
        input_samples[i] = amplitude * sin(2 * 3.14159 * frequency * t);
    }

    int num_bytes;
    unsigned char *encoded_data = (unsigned char *)malloc(MAX_PACKET_SIZE);
    opus_int16 *decoded_samples = (opus_int16 *)malloc(sizeof(opus_int16) * FRAME_SIZE);

    /* Encode and decode audio frames */
    int num_frames = num_samples / FRAME_SIZE;
    for (int i = 0; i < num_frames; ++i) {
        /* Encode audio frame */
        num_bytes = opus_encode(encoder, input_samples + (i * FRAME_SIZE), FRAME_SIZE, encoded_data, MAX_PACKET_SIZE);
        if (num_bytes < 0) {
            fprintf(stderr, "Failed to encode frame: %s\n", opus_strerror(num_bytes));
            break;
        }
		printf("encode: %d -> %d\n", FRAME_SIZE, num_bytes);

        /* Decode audio frame */
        int num_samples = opus_decode(decoder, encoded_data, num_bytes, decoded_samples, FRAME_SIZE, 0);
        if (num_samples < 0) {
            fprintf(stderr, "Failed to decode frame: %s\n", opus_strerror(num_samples));
            break;
        }

        /* Process decoded samples (e.g., play audio) */
        /* For simplicity, we are not doing any processing here */
    }

    /* Clean up resources */
    free(input_samples);
    free(encoded_data);
    free(decoded_samples);
    opus_encoder_destroy(encoder);
    opus_decoder_destroy(decoder);

    return EXIT_SUCCESS;
}