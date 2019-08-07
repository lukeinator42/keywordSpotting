// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* ===========================================================================
 * @file  EmbeddingNetwork.cpp
 * @brief Neural network that returns embeddings for audio.
 * =========================================================================== */

#include <keywordSpotter/lib/EmbeddingNetwork.h>


EmbeddingNetwork::EmbeddingNetwork(std::string graph_fn, std::string checkpoint_fn) {
    
    TF_CHECK_OK(tensorflow::NewSession(options, &sess));
    TF_CHECK_OK(LoadModel(sess, graph_fn, checkpoint_fn));
}

EmbeddingNetwork::~EmbeddingNetwork() {

}

tensorflow::Status EmbeddingNetwork::LoadModel(tensorflow::Session *sess, std::string graph_fn,
                             std::string checkpoint_fn) {
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

std::string EmbeddingNetwork::getMsg() {
    return this->msg;
}

Embedding EmbeddingNetwork::getSpectrogramEmbedding(std::vector<float> s) {
    tensorflow::TensorShape data_shape({1,(long long) s.size()});
    tensorflow::Tensor data(tensorflow::DT_FLOAT, data_shape);
    
    auto data_ = data.tensor<float, 2>();

    
    for(int i=0; i<s.size(); i++)
        data_(0, i) = s[i];

    tensor_dict feed_dict = {
        {"input_2:0", data},
    };

    std::vector<tensorflow::Tensor> outputs;

    TF_CHECK_OK(sess->Run(feed_dict, {"lstm_4/transpose_1:0"}, {}, &outputs));

    auto output_c = outputs[0].flat<float>();

    Embedding res;

    for(int i=0; i<output_c.size(); i++)
        res.push_back(output_c.data()[i]);

    return res;
}