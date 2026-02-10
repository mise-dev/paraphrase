#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QHotkey>
#include <QTextEdit>
#include <iostream>
#include <chrono>
#include <QClipboard>
#include <QScreen>
#include "llama.h"
#include <filesystem>
#include <fstream>
static void llama_log_callback_null(ggml_log_level level, const char * text, void * user_data) {
    (void) level;
    (void) text;
    (void) user_data;
}
void paraphrase_ai(std::string& text, std::string model_str){
    std::string system_msg = "You are a helpful assistant that rewrites text in different words. Output only the rewritten text, nothing else.";
    std::string user_msg = "Rewrite this text: \"" + text + "\"";

    std::string prompt = 
    "<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n" +
    system_msg + 
    "<|eot_id|><|start_header_id|>user<|end_header_id|>\n" +
    user_msg + 
    "<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n";
    text="";
    // number of layers to offload to the GPU
    int ngl = 0;
    // number of tokens to predict
    int n_predict = 32;

    // load dynamic backends

    ggml_backend_load_all();
    llama_log_set(llama_log_callback_null, NULL);

    // initialize the model

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = ngl;

    llama_model* model = llama_model_load_from_file(model_str.c_str(), model_params);

    if (model == NULL) {
        fprintf(stderr , "%s: error: unable to load model\n" , __func__);
    }

    const llama_vocab * vocab = llama_model_get_vocab(model);
    // tokenize the prompt

    // find the number of tokens in the prompt
    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, true, true);

    // allocate space for the tokens and tokenize the prompt
    std::vector<llama_token> prompt_tokens(n_prompt);
    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0) {
        fprintf(stderr, "%s: error: failed to tokenize the prompt\n", __func__);
    }
    // initialize the context

    llama_context_params ctx_params = llama_context_default_params();
    // n_ctx is the context size
    ctx_params.n_ctx = n_prompt + n_predict - 1;
    // n_batch is the maximum number of tokens that can be processed in a single call to llama_decode
    ctx_params.n_batch = n_prompt;
    // enable performance counters
    ctx_params.no_perf = false;

    llama_context * ctx = llama_init_from_model(model, ctx_params);

    if (ctx == NULL) {
        fprintf(stderr , "%s: error: failed to create the llama_context\n" , __func__);
    }

    // initialize the sampler
    llama_sampler * smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    // print the prompt token-by-token

    for (auto id : prompt_tokens) {
        char buf[128];
        int n = llama_token_to_piece(vocab, id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);

        }
        //std::string s(buf, n);
        //printf("%s", s.c_str());
    }

    // prepare a batch for the prompt

    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    if (llama_model_has_encoder(model)) {
        if (llama_encode(ctx, batch)) {
            fprintf(stderr, "%s : failed to eval\n", __func__);

        }

        llama_token decoder_start_token_id = llama_model_decoder_start_token(model);
        if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
            decoder_start_token_id = llama_vocab_bos(vocab);
        }

        batch = llama_batch_get_one(&decoder_start_token_id, 1);
    }
     // main loop

    int n_decode = 0;
    llama_token new_token_id;

    for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + n_predict; ) {
        // evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch)) {
            fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);

        }

        n_pos += batch.n_tokens;

        // sample the next token
        {
            new_token_id = llama_sampler_sample(smpl, ctx, -1);

            // is it an end of generation?
            if (llama_vocab_is_eog(vocab, new_token_id)) {
                break;
            }

            char buf[128];
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) {
                fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
    
            }
            std::string s(buf, n);
            //printf("%s", s.c_str());
            text+=s.c_str();
            fflush(stdout);

            // prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);

            n_decode += 1;
        }
    }
    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();

}
std::map<std::string, std::string> parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    std::map<std::string, std::string> config;
    std::string line;
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }
    
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Remove comments (everything after #)
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Find the delimiter (= or :)
        size_t delimPos = line.find('=');
        if (delimPos == std::string::npos) {
            delimPos = line.find(':');
        }
        
        if (delimPos != std::string::npos) {
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);
            
            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config[key] = value;
        } else {
            std::cerr << "Warning: Invalid config format at line " 
                      << lineNum << ": " << line << std::endl;
        }
    }
    
    file.close();
    return config;
}
namespace fs = std::filesystem;
int main(int argc, char *argv[]){
    auto config = parseConfigFile("config.conf");
    std::string model_str = config["model"];
    std::string hotkey_str = config["hotkey"];
    std::string backimg = config["background_img"];
    int overwrite_xy = std::stoi(config["overwrite_xy"]);
    QApplication app(argc, argv);
    QHotkey *hotkey = new QHotkey(QKeySequence(QString::fromStdString(hotkey_str)), true, &app);
    QWidget *window = new QWidget();
    QClipboard *clipboard = QApplication::clipboard();
    window->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    window->setStyleSheet("background: #000000ff; border: 1px solid #555;");
    int width = std::stoi(config["width"]);
    int height = std::stoi(config["height"]);
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;
    if(overwrite_xy==1){
        x=std::stoi(config["x"]);
        y=std::stoi(config["y"]);
    }
    window->setGeometry(x, y, width, height);
    if (fs::exists(backimg))window->setStyleSheet("background-image: url(" + QString::fromStdString(backimg) + ");");
    QTextEdit *textEdit;
    textEdit = new QTextEdit(window);
    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(textEdit);
    QPalette palette = textEdit->palette();
    palette.setColor(QPalette::Text, QColor(QString::fromStdString("#" + config["font_color"])));
    palette.setColor(QPalette::Base, Qt::white);
    textEdit->setPalette(palette);
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setItalic(false);
    font.setUnderline(false);
    textEdit->setFont(font);
    QObject::connect(hotkey, &QHotkey::activated, [window,textEdit,clipboard,model_str](){
        static int window_show = 0;
        window_show=(window_show+1)%2;
        if(window_show){
            window->show();
            window->activateWindow();
            window->raise();
        }
        else{
            window->hide();
            QString qstr = textEdit->toPlainText();
            textEdit->clear();
            
            std::string str = qstr.toStdString();
            paraphrase_ai(str, model_str);
            str.erase(str.length() - 1, 1);
            str.erase(0, 1);
            qstr=QString::fromStdString(str);
            
            clipboard->setText(qstr);
        }
    });

    return app.exec();
}