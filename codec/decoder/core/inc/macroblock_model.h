#ifndef _MACROBLOCK_MODEL_H_
#define _MACROBLOCK_MODEL_H_

#include "array_nd.h"
#include "compression_stream.h"

const char *billEnumToName(int en);
#define GENERATE_LZMA_MODE_FILE 1
#if GENERATE_LZMA_MODE_FILE
enum {
    PIP_DEFAULT_TAG,
    PIP_SKIP_TAG,
    PIP_SKIP_END_TAG,
    PIP_CBPC_TAG,
    PIP_CBPL_TAG,
    PIP_LAST_MB_TAG,
    PIP_QPL_TAG,
    PIP_MB_TYPE_TAG,
    PIP_REF_TAG,
    PIP_8x8_TAG,
    PIP_16x16_TAG,
    PIP_PRED_TAG,
    PIP_PRED_MODE_TAG,
    PIP_SUB_MB_TAG,
    PIP_MVX_TAG,
    PIP_MVY_TAG,
    PIP_LDC_TAG,
    PIP_CRDC_TAG,
    PIP_LAST_NONVAR_TAG
};
const int PIP_AC_STEP = 1;
enum {
    PIP_LAC_TAG0 = PIP_LAST_NONVAR_TAG,
    PIP_CRAC_TAG0 = PIP_LAST_NONVAR_TAG + 16,
    PIP_LAST_VAR_TAG = PIP_LAST_NONVAR_TAG + 32
};
enum {
    PIP_PREV_PRED_TAG = PIP_LAST_VAR_TAG,
    PIP_PREV_PRED_MODE_TAG,
    PIP_NZC_TAG,
    NUM_TOTAL_TAGS
};
#define BILLING
extern double bill[NUM_TOTAL_TAGS];
extern int curBillTag;
#else
enum {
    PIP_DEFAULT_TAG=1,
    PIP_SKIP_TAG=1,
    PIP_SKIP_END_TAG=1,
    PIP_CBPC_TAG=1,
    PIP_CBPL_TAG=1,
    PIP_LAST_MB_TAG=1,
    PIP_QPL_TAG=1,
    PIP_QPC_TAG=1,
    PIP_MB_TYPE_TAG=1,
    PIP_REF_TAG=1,
    PIP_8x8_TAG=1,
    PIP_16x16_TAG=1,
    PIP_PRED_TAG=1,
    PIP_PRED_MODE_TAG=1,
    PIP_SUB_MB_TAG=1,
    PIP_MVX_TAG=1,
    PIP_MVY_TAG=1,
    PIP_LDC_TAG=1,
    PIP_CRDC_TAG=1,
};
const int PIP_AC_STEP = 0;
enum {
    PIP_LAC_TAG0 = 1,
    PIP_CRAC_TAG0 = 1,
};
enum {
    PIP_PREV_PRED_TAG = 1,
    PIP_PREV_PRED_MODE_TAG=1,
    PIP_NZC_TAG=1
};
#endif
struct DecodedMacroblock;
struct FreqImage;
namespace Nei {
    enum NeighborType{LEFT, ABOVE, ABOVELEFT, ABOVERIGHT, PAST, NUMNEIGHBORS};
}
struct Neighbors {

    const DecodedMacroblock *n[Nei::NUMNEIGHBORS];
    const DecodedMacroblock *operator[](Nei::NeighborType index) const {return n[index];}
    void init(const FreqImage *f, int x, int y);
};

namespace WelsDec{
    struct TagWelsDecoderContext;
    typedef struct TagWelsDecoderContext *PWelsDecoderContext;
}

class MacroblockModel {

    DecodedMacroblock *mb;
    WelsDec::PWelsDecoderContext pCtx;
    Neighbors n;
    Sirikata::Array3d<DynProb, 32, 2, 15> mbTypePriors; // We could use just 8 bits for I Slices
    Sirikata::Array2d<DynProb, 8, 8> lumaI16x16ModePriors;
    Sirikata::Array2d<DynProb, 8, 8> chromaI8x8ModePriors;
     Sirikata::Array3d<DynProb,
        257, // prev frame or neighbor 4x16 + 16x4
        16,//mbType
        256 // number of nonzero values possible
        > numNonZerosLumaPriors;
    Sirikata::Array3d<DynProb,
                      129, // prev frame num nonzeros
                      16, // mbType
                      128> numNonZerosChromaPriors;
    Sirikata::Array4d<DynProb,
        17,
        17,
        17,
        16> numSubNonZerosLumaPriors;
    Sirikata::Array4d<DynProb,
        17,//FIXME
        17,//FIXME
        17, //mbType
        16> numSubNonZerosChromaPriors;
public:
    void initCurrentMacroblock(DecodedMacroblock *curMb, WelsDec::PWelsDecoderContext pCtx,
                               const FreqImage *, int mbx, int mby);

    Branch<4> getMacroblockTypePrior();
    std::pair<Sirikata::Array1d<DynProb, 8>::Slice, uint32_t> getLumaI16x16ModePrior();
    std::pair<Sirikata::Array1d<DynProb, 8>::Slice, uint32_t> getChromaI8x8ModePrior();
    Sirikata::Array1d<DynProb, 256>::Slice getLumaNumNonzerosPrior();
    Sirikata::Array1d<DynProb, 128>::Slice getChromaNumNonzerosPrior();
    Branch<8> getLumaNumNonzerosPriorBranch() {
        return getLumaNumNonzerosPrior().slice<1, 256>();
    }
    Branch<7> getChromaNumNonzerosPriorBranch() {
        return getChromaNumNonzerosPrior().slice<1, 128>();
    }
    Sirikata::Array1d<DynProb, 16>::Slice getSubLumaNumNonzerosPrior(uint8_t i, uint8_t runningCount);
    Sirikata::Array1d<DynProb, 16>::Slice getSubChromaNumNonzerosPrior(uint8_t i, uint8_t runningCount);
    uint16_t getAndUpdateMacroblockLumaNumNonzeros(); // between 0 and 256, inclusive
    uint8_t getAndUpdateMacroblockChromaNumNonzeros(); // between 0 and 128, inclusive
    int encodeMacroblockType(int welsType);
    int decodeMacroblockType(int storedType);
    uint8_t get4x4NumNonzeros(uint8_t index, uint8_t color/*0 for Y 1 for U, 2 for V*/) const;
};


#endif
