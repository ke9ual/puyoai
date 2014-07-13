#include "audio/audio_commentator.h"

#include <iostream>
#include "glog/logging.h"

#include "audio/speaker.h"
#include "duel/game_state.h"
#include "gui/commentator.h"

using namespace std;

AudioCommentator::AudioCommentator(const Commentator* commentator) :
    commentator_(commentator)
{
}

AudioCommentator::~AudioCommentator()
{
}

void AudioCommentator::newGameWillStart()
{
    lock_guard<mutex> lock(mu_);
    texts_.push_back("さあ、ゲームの始まりです。");
}

void AudioCommentator::onUpdate(const GameState&)
{
}

void AudioCommentator::gameHasDone()
{
}

SpeakRequest AudioCommentator::requestSpeak()
{
    {
        lock_guard<mutex> lock(mu_);
        if (!texts_.empty()) {
            string textToSpeak = texts_.back();
            texts_.clear();
            return SpeakRequest { textToSpeak };
        }
    }

    vector<tuple<int, int, string>> candidates;

    string playerNames[] = {"ワンピー", "ツーピー"};

    CommentatorResult result = commentator_->result();
    for (int pi = 0; pi < 2; ++pi) {
        if (result.firingChain[pi].chains() > 0) {
            if (!firing_[pi]) {
                int chain = result.firingChain[pi].chains();
                int score = result.firingChain[pi].score();
                if (chain == 1) {
                    if (score >= 70 * 10) {
                        candidates.emplace_back(score - penalty_[pi], pi, playerNames[pi] + "、イバラを発火");
                    }
                    if (score <= 100) {
                        candidates.emplace_back(3 - penalty_[pi], pi, playerNames[pi] + "、単発で整地");
                    }
                } else {
                    string s = playerNames[pi] + to_string(chain) + "連鎖を発火。";
                    if (score / 70 >= 6)
                        s += "オジャマプヨ " + to_string(score / 70) + "個が発生";
                    if (firing_[1 - pi]) {
                        int opponentScore = result.firingChain[1 - pi].score();
                        if (score > opponentScore + 70 * 30) {
                            s += "十分な大きさがある。";
                        } else if (score > opponentScore - 70 * 12) {
                            s += "なんとか対応。";
                        } else {
                            s += "これでは小さいか。";
                        }
                    }
                    candidates.emplace_back(score, pi, s);
                }
            }

            firing_[pi] = true;
        } else {
            firing_[pi] = false;
        }

        if (result.fireableMainChain[pi].chains() > 0) {
            int chain = result.fireableMainChain[pi].chains();
            int priority = chain - penalty_[pi];
            candidates.emplace_back(priority, pi, playerNames[pi] + to_string(chain) + "連鎖を保持中。");
        }
    }

    if (candidates.empty())
        return SpeakRequest();

    const auto& st = *max_element(candidates.begin(), candidates.end());
    penalty_[1 - get<1>(st)] = 0;
    penalty_[get<1>(st)]++;
    return SpeakRequest(get<2>(st));
}
