#include <iostream>
#include <fstream>
#include <string>
#include <keywordSpotter/EmbeddingNetwork.h>
#include <sndfile.h>
#include <keywordSpotter/utils/WavUtils.h>
#include <keywordSpotter/sound/Sound.h>
#include <keywordSpotter/features/Spectrogram.h>
#include <tensorflow/core/protobuf/meta_graph.pb.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/public/session_options.h>

using namespace std;

typedef vector<pair<string, tensorflow::Tensor>> tensor_dict;

tensorflow::Status LoadModel(tensorflow::Session *sess, std::string graph_fn,
                             std::string checkpoint_fn = "") {
  tensorflow::Status status;

  // Read in the protobuf graph we exported
  tensorflow::MetaGraphDef graph_def;
  status = ReadBinaryProto(tensorflow::Env::Default(), graph_fn, &graph_def);
  if (status != tensorflow::Status::OK()) return status;

  // create the graph
  status = sess->Create(graph_def.graph_def());
  if (status != tensorflow::Status::OK()) return status;

  // restore model from checkpoint, iff checkpoint is given
  if (checkpoint_fn != "") {
    tensorflow::Tensor checkpointPathTensor(tensorflow::DT_STRING,
                                            tensorflow::TensorShape());
    checkpointPathTensor.scalar<std::string>()() = checkpoint_fn;

    tensor_dict feed_dict = {
        {graph_def.saver_def().filename_tensor_name(), checkpointPathTensor}};
    status = sess->Run(feed_dict, {}, {graph_def.saver_def().restore_op_name()},
                       nullptr);
    if (status != tensorflow::Status::OK()) return status;
  } else {
    // virtual Status Run(const std::vector<std::pair<string, Tensor> >& inputs,
    //                  const std::vector<string>& output_tensor_names,
    //                  const std::vector<string>& target_node_names,
    //                  std::vector<Tensor>* outputs) = 0;
    status = sess->Run({}, {}, {"init"}, nullptr);
    if (status != tensorflow::Status::OK()) return status;
  }

  return tensorflow::Status::OK();
}

int main(int argc, char ** argv) {

    const string graph_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/librispeech_lstm_embedding_model.meta";
    const string checkpoint_fn = "/home/ikubu/PycharmProjects/acoustic_embeddings/export/librispeech_lstm_embedding_model";
    
    if (argc != 2) {
        printf("\nUsage :\n\n    <executable name>  <input file>\n") ;
        exit(0);
    }

    string inpath(argv[1]);

    WavUtils wavUtils;

    Sound buffer;

    int nFFT = 128;
    int winLen = 400;
    int hopSize = 160;
    double minFreq = 50;
    double maxFreq = 8000;
    double sampleRate = 16000;
    bool logScale = true;

    Spectrogram spectrogram(nFFT, winLen, hopSize, minFreq, maxFreq, sampleRate, logScale);

    wavUtils.readFile(inpath, buffer);
    vector< vector<double> > s = spectrogram.computeSpectrogram(buffer.getAudio());

    tensorflow::Session *sess;
    tensorflow::SessionOptions options;
    TF_CHECK_OK(tensorflow::NewSession(options, &sess));
    TF_CHECK_OK(LoadModel(sess, graph_fn, checkpoint_fn));

    tensorflow::TensorShape data_shape({1, s.size(), s[0].size()});
    tensorflow::Tensor data(tensorflow::DT_FLOAT, data_shape);
    
    auto data_ = data.tensor<float, 3>();

    for(int i=0; i<s.size(); i++)
        for(int j=0; j<s[i].size(); j++)
            data_(0, i, j) = s[i][j];

    tensor_dict feed_dict = {
        {"input_1:0", data},
    };

    vector<tensorflow::Tensor> outputs;

    TF_CHECK_OK(sess->Run(feed_dict, {"lstm_4/transpose_1:0"}, {}, &outputs));

    cout << "input           " << data.DebugString() << std::endl;
    cout << "output          " << outputs[0].DebugString() << std::endl;

    return 0;
}