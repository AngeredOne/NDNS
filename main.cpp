#include <iostream>
#include "VoiceClient.h"

int main(int, char **)
{
    //NO DISCORD NO SKYPE
    std::cout << "<---===:::::::::::::::::::::===--->\n";
    std::cout << "<---===       N_D_N_S       ===--->\n";
    std::cout << "<---=== NO DISCORD NO SKYPE ===--->\n";
    std::cout << "<---===:::::::::::::::::::::===--->\n";

    std::cout << "Enter IP: ";
    std::string remoteIP;
    getline(std::cin, remoteIP);
    VoiceClient *vclient = new VoiceClient(remoteIP);

    MicroTranslator *translator = new MicroTranslator;
    translator->SetUp(vclient);
    translator->start();
    while (1)
        ;
}
