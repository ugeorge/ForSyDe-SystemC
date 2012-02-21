
#include "ReadBitstreamAndExtractFrames.hpp"
#include "ProcessChanuleZeroLeft.hpp"
#include "ProcessChanuleZeroRight.hpp"
#include "ProcessChanuleOneLeft.hpp"
#include "ProcessChanuleOneRight.hpp"
#include "Merge.hpp"
#include "ProcessGranuleZero.hpp"
#include "ProcessGranuleOne.hpp"

using namespace ForSyDe::SDF;
using namespace std;

ostream& operator <<(ostream &os,const ChanuleSamples &obj)
{
      //~ os<<obj.strVal;
      return os;
}
ostream& operator <<(ostream &os,const ChanuleData &obj)
{
      //~ os<<obj.strVal;
      return os;
}
ostream& operator <<(ostream &os,const GranuleData &obj)
{
      //~ os<<obj.strVal;
      return os;
}
ostream& operator <<(ostream &os,const FrameSideInfo &obj)
{
      //~ os<<obj.strVal;
      return os;
}
ostream& operator <<(ostream &os,const FrameHeader &obj)
{
      //~ os<<obj.strVal;
      return os;
}
typedef comb<float,InputType> ReadBitstreamAndExtractFrames;

typedef comb3<FrameHeader,FrameSideInfo,ChanuleData,
              ChanuleSamples> ProcessChanule;

typedef comb4<FrameHeader,FrameSideInfo,GranuleData,t_sf_band_indices*,
    tuple<
        vector<FrameHeader>,
        vector<FrameSideInfo>,
        vector<ChanuleData>,
        vector<FrameHeader>,
        vector<FrameSideInfo>,
        vector<ChanuleData>
    >> ProcessGranule;

typedef unzipN<
        FrameHeader,
        FrameSideInfo,
        ChanuleData,
        FrameHeader,
        FrameSideInfo,
        ChanuleData
    > GranuelUnzipper;

typedef unzipN<
        float,
        bool,
        FrameHeader,
        FrameHeader,
        FrameSideInfo,
        GranuleData,
        t_sf_band_indices*,
        FrameHeader,
        FrameSideInfo,
        GranuleData,
        t_sf_band_indices*
    > InputUnzipper;

typedef zipN<
    ChanuleSamples,
    ChanuleSamples,
    bool,
    FrameHeader,
    ChanuleSamples,
    ChanuleSamples
    > MergeZipper;

SC_MODULE(Top)
{
public:
    /* Actors */
    ReadBitstreamAndExtractFrames *a_ReadBitstreamAndExtractFrames;
    InputUnzipper *a_InputUnzipper;
    delayn<float> *a_DummyLoopDelay;
    ProcessChanule *a_ProcessChanule0Left;
    MergeZipper *a_MergeZipper;
    Merge *a_Merge;
    ProcessGranule *a_ProcessGranule0, *a_ProcessGranule1;
    GranuelUnzipper *a_Granuel0Unzipper, *a_Granuel1Unzipper;
    ProcessChanule *a_ProcessChanule0Right;
    ProcessChanule *a_ProcessChanule1Right;
    ProcessChanule *a_ProcessChanule1Left;

    /* Channels */
    SDF2SDF<float> *dummyloopi;
    SDF2SDF<float> *dummyloopo;
    SDF2SDF<InputType> *zippedInput;
    SDF2SDF<bool> *lastFrame;
    SDF2SDF<FrameHeader> *headerGranule0;
    SDF2SDF<FrameHeader> *headerGranule1;
    SDF2SDF<FrameSideInfo> *sideInfoGranule0;
    SDF2SDF<FrameSideInfo> *sideInfoGranule1;
    SDF2SDF<GranuleData> *granuleData0;
    SDF2SDF<GranuleData> *granuleData1;
    SDF2SDF<t_sf_band_indices*> *bandIndicesGranule0;
    SDF2SDF<t_sf_band_indices*> *bandIndicesGranule1;
    SDF2SDF<GranuleType> *zippedGranuel0Out;
    SDF2SDF<GranuleType> *zippedGranuel1Out;
    SDF2SDF<FrameHeader> *headerMerge;
    SDF2SDF<FrameHeader> *headerChanule0Left;
    SDF2SDF<FrameHeader> *headerChanule0Right;
    SDF2SDF<FrameSideInfo> *sideInfoChanule0Left;
    SDF2SDF<FrameSideInfo> *sideInfoChanule0Right;
    SDF2SDF<ChanuleData> *chanuleData0Left;
    SDF2SDF<ChanuleData> *chanuleData0Right;
    SDF2SDF<FrameHeader> *headerChanule1Left;
    SDF2SDF<FrameHeader> *headerChanule1Right;
    SDF2SDF<FrameSideInfo> *sideInfoChanule1Left;
    SDF2SDF<FrameSideInfo> *sideInfoChanule1Right;
    SDF2SDF<ChanuleData> *chanuleData1Left;
    SDF2SDF<ChanuleData> *chanuleData1Right;
    SDF2SDF<ChanuleSamples> *samples_0_Left;
    SDF2SDF<ChanuleSamples> *samples_0_Right;
    SDF2SDF<ChanuleSamples> *samples_1_Left;
    SDF2SDF<ChanuleSamples> *samples_1_Right;
    SDF2SDF<MergeType> *zippedMerge;

    SC_CTOR(Top)
    {
        /* Create FIFOs */
        dummyloopi = new SDF2SDF<float>("dummyloopi",1);
        dummyloopo = new SDF2SDF<float>("dummyloopo",1);
        zippedInput = new SDF2SDF<InputType>("zippedInput",1);
        lastFrame = new SDF2SDF<bool>("lastFrame",1);
        headerGranule0 = new SDF2SDF<FrameHeader>("headerGranule0",1);
        headerGranule1 = new SDF2SDF<FrameHeader>("headerGranule1",1);
        sideInfoGranule0 = new SDF2SDF<FrameSideInfo>("sideInfoGranule0",1);
        sideInfoGranule1 = new SDF2SDF<FrameSideInfo>("sideInfoGranule1",1);
        granuleData0 = new SDF2SDF<GranuleData>("granuleData0",1);
        granuleData1 = new SDF2SDF<GranuleData>("granuleData1",1);
        bandIndicesGranule0 = new SDF2SDF<t_sf_band_indices*>("bandIndicesGranule0",1);
        bandIndicesGranule1 = new SDF2SDF<t_sf_band_indices*>("bandIndicesGranule1",1);
        zippedGranuel0Out = new SDF2SDF<GranuleType>("zippedGranuel0Out",1);
        zippedGranuel1Out = new SDF2SDF<GranuleType>("zippedGranuel1Out",1);;
        headerMerge = new SDF2SDF<FrameHeader>("headerMerge",1);
        headerChanule0Left = new SDF2SDF<FrameHeader>("headerChanule0Left",1);
        headerChanule0Right = new SDF2SDF<FrameHeader>("headerChanule0Right",1);
        sideInfoChanule0Left = new SDF2SDF<FrameSideInfo>("sideInfoChanule0Left",1);
        sideInfoChanule0Right = new SDF2SDF<FrameSideInfo>("sideInfoChanule0Right",1);
        chanuleData0Left = new SDF2SDF<ChanuleData>("chanuleData0Left",1);
        chanuleData0Right = new SDF2SDF<ChanuleData>("chanuleData0Right",1);
        headerChanule1Left = new SDF2SDF<FrameHeader>("headerChanule1Left",1);
        headerChanule1Right = new SDF2SDF<FrameHeader>("headerChanule1Right",1);
        sideInfoChanule1Left = new SDF2SDF<FrameSideInfo>("sideInfoChanule1Left",1);
        sideInfoChanule1Right = new SDF2SDF<FrameSideInfo>("sideInfoChanule1Right",1);
        chanuleData1Left = new SDF2SDF<ChanuleData>("chanuleData1Left",1);
        chanuleData1Right = new SDF2SDF<ChanuleData>("chanuleData1Right",1);
        samples_0_Left = new SDF2SDF<ChanuleSamples>("samples_0_Left",1);
        samples_0_Right = new SDF2SDF<ChanuleSamples>("samples_0_Right",1);
        samples_1_Left = new SDF2SDF<ChanuleSamples>("samples_1_Left",1);
        samples_1_Right = new SDF2SDF<ChanuleSamples>("samples_1_Right",1);
        zippedMerge = new SDF2SDF<MergeType>("zippedMerge",1);

        a_ReadBitstreamAndExtractFrames = new ReadBitstreamAndExtractFrames("ReadBitstreamAndExtractFrames",ReadBitstreamAndExtractFrames_func,1,1);
        a_ReadBitstreamAndExtractFrames->iport(*dummyloopo);
        a_ReadBitstreamAndExtractFrames->oport(*zippedInput);
        //
        vector<unsigned> inputUnzipperRates = {1,1,1,1,1,1,1,1,1,1,1};
        a_InputUnzipper = new InputUnzipper("InputUnzipper",inputUnzipperRates);
        a_InputUnzipper->iport(*zippedInput);
        get<0>(a_InputUnzipper->oport)(*dummyloopi);
        get<1>(a_InputUnzipper->oport)(*lastFrame);
        get<2>(a_InputUnzipper->oport)(*headerMerge);
        get<3>(a_InputUnzipper->oport)(*headerGranule0);
        get<4>(a_InputUnzipper->oport)(*sideInfoGranule0);
        get<5>(a_InputUnzipper->oport)(*granuleData0);
        get<6>(a_InputUnzipper->oport)(*bandIndicesGranule0);
        get<7>(a_InputUnzipper->oport)(*headerGranule1);
        get<8>(a_InputUnzipper->oport)(*sideInfoGranule1);
        get<9>(a_InputUnzipper->oport)(*granuleData1);
        get<10>(a_InputUnzipper->oport)(*bandIndicesGranule1);
        //
        a_DummyLoopDelay = new delayn<float>("DummyLoopDelay",1,1);
        a_DummyLoopDelay->iport(*dummyloopi);
        a_DummyLoopDelay->oport(*dummyloopo);
        
        a_ProcessChanule0Left = new ProcessChanule("ProcessChanuleZeroLeft0",ProcessChanuleZeroLeft_func,1,1,1,1);
        a_ProcessChanule0Left->iport1(*headerChanule0Left);
        a_ProcessChanule0Left->iport2(*sideInfoChanule0Left);
        a_ProcessChanule0Left->iport3(*chanuleData0Left);
        a_ProcessChanule0Left->oport(*samples_0_Left);
        
        vector<unsigned> mergeZipperRates = {1,1,1,1,1,1};
        a_MergeZipper = new MergeZipper("MergeZipper",mergeZipperRates);
        get<0>(a_MergeZipper->iport)(*samples_1_Right);
        get<1>(a_MergeZipper->iport)(*samples_0_Left);
        get<2>(a_MergeZipper->iport)(*lastFrame);
        get<3>(a_MergeZipper->iport)(*headerMerge);
        get<4>(a_MergeZipper->iport)(*samples_1_Left);
        get<5>(a_MergeZipper->iport)(*samples_0_Right);
        a_MergeZipper->oport(*zippedMerge);
        //
        a_Merge = new Merge("Merge");
        a_Merge->iport(*zippedMerge);
        
        a_ProcessGranule0 = new ProcessGranule("ProcessGranuleZero0",ProcessGranuleZero_func,1,1,1,1,1);
        a_ProcessGranule0->iport1(*headerGranule0);
        a_ProcessGranule0->iport2(*sideInfoGranule0);
        a_ProcessGranule0->iport3(*granuleData0);
        a_ProcessGranule0->iport4(*bandIndicesGranule0);
        a_ProcessGranule0->oport(*zippedGranuel0Out);
        //
        vector<unsigned> granuelUnzipperRates = {1,1,1,1,1,1};
        a_Granuel0Unzipper = new GranuelUnzipper("GranuelUnzipper0",granuelUnzipperRates);
        a_Granuel0Unzipper->iport(*zippedGranuel0Out);
        get<0>(a_Granuel0Unzipper->oport)(*headerChanule0Left);
        get<1>(a_Granuel0Unzipper->oport)(*sideInfoChanule0Left);
        get<2>(a_Granuel0Unzipper->oport)(*chanuleData0Left);
        get<3>(a_Granuel0Unzipper->oport)(*headerChanule0Right);
        get<4>(a_Granuel0Unzipper->oport)(*sideInfoChanule0Right);
        get<5>(a_Granuel0Unzipper->oport)(*chanuleData0Right);

        a_ProcessGranule1 = new ProcessGranule("ProcessGranuleOne",ProcessGranuleOne_func,1,1,1,1,1);
        a_ProcessGranule1->iport1(*headerGranule1);
        a_ProcessGranule1->iport2(*sideInfoGranule1);
        a_ProcessGranule1->iport3(*granuleData1);
        a_ProcessGranule1->iport4(*bandIndicesGranule1);
        a_ProcessGranule1->oport(*zippedGranuel1Out);
        //
        a_Granuel1Unzipper = new GranuelUnzipper("GranuelUnzipper1",granuelUnzipperRates);
        a_Granuel1Unzipper->iport(*zippedGranuel1Out);
        get<0>(a_Granuel1Unzipper->oport)(*headerChanule1Left);
        get<1>(a_Granuel1Unzipper->oport)(*sideInfoChanule1Left);
        get<2>(a_Granuel1Unzipper->oport)(*chanuleData1Left);
        get<3>(a_Granuel1Unzipper->oport)(*headerChanule1Right);
        get<4>(a_Granuel1Unzipper->oport)(*sideInfoChanule1Right);
        get<5>(a_Granuel1Unzipper->oport)(*chanuleData1Right);
        
        a_ProcessChanule0Right = new ProcessChanule("ProcessChanuleZeroRight0",ProcessChanuleZeroRight_func,1,1,1,1);
        a_ProcessChanule0Right->iport1(*headerChanule0Right);
        a_ProcessChanule0Right->iport2(*sideInfoChanule0Right);
        a_ProcessChanule0Right->iport3(*chanuleData0Right);
        a_ProcessChanule0Right->oport(*samples_0_Right);
        
        a_ProcessChanule1Right = new ProcessChanule("ProcessChanuleOneRight0",ProcessChanuleOneRight_func,1,1,1,1);
        a_ProcessChanule1Right->iport1(*headerChanule1Right);
        a_ProcessChanule1Right->iport2(*sideInfoChanule1Right);
        a_ProcessChanule1Right->iport3(*chanuleData1Right);
        a_ProcessChanule1Right->oport(*samples_1_Right);
        
        a_ProcessChanule1Left = new ProcessChanule("ProcessChanuleOneLeft0",ProcessChanuleOneLeft_func,1,1,1,1);
        a_ProcessChanule1Left->iport1(*headerChanule1Left);
        a_ProcessChanule1Left->iport2(*sideInfoChanule1Left);
        a_ProcessChanule1Left->iport3(*chanuleData1Left);
        a_ProcessChanule1Left->oport(*samples_1_Left);
    }

public:
    void start_of_simulation()
    {
        ForSyDe::CoMPSoCExport dumper;
        dumper.traverse(this);
        dumper.printXML("gen/mp3decoder.xml");
        dumper.printSrc("./","gen/");
    }
};