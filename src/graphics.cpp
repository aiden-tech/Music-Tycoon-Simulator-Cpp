#include "../headers/graphics.h"
#include "../headers/helper.h"
#include "imgui.h"

EventLog gameLog;

void EventLog::Add(const std::string &message) {
  logs.push_front(message);
  if (logs.size() > 50)
    logs.pop_back();
}

void EventLog::DrawWindow() {
  if (ImGui::Begin("News Feed")) {
    for (const auto &log : logs) {
      ImGui::TextWrapped("%s", log.c_str());
      ImGui::Separator();
    }
  }
  ImGui::End();
}

std::string BeginDropDownMenu(bool IsOpen) {
  static int selected_idx = 0;
  const char *items[] = {"Pop",  "Rock",      "Hip-Hop", "R&B",
                         "Jazz", "Classical", "Other"};

  // 1. Use 'if' instead of 'while' to prevent the app from freezing.
  if (IsOpen) {
    // 2. Only call EndCombo() if BeginCombo() returns true.
    if (ImGui::BeginCombo("Genre", items[selected_idx])) {
      for (int n = 0; n < (int)IM_ARRAYSIZE(items); n++) {
        const bool is_selected = (selected_idx == n);

        if (ImGui::Selectable(items[n], is_selected)) {
          selected_idx = n;
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      // Move EndCombo inside the successful BeginCombo block
      ImGui::EndCombo();
    }
  }

  // 3. Always return the current selection, whether the menu was drawn this
  // frame or not.
  return items[selected_idx];
}

void DrawStudioWindow(Player &player, std::vector<Song> &songsMade,
                      std::vector<Song> &songsReleased,
                      std::vector<Album> &albumsReleased) {
  ImGui::Begin("Production Studio");

  // --- 1. Header & Stats ---
  ImGui::TextColored(ImVec4(0, 1, 0, 1), "Artist: %s", player.name.c_str());
  ImGui::SameLine(ImGui::GetWindowWidth() - 150);
  ImGui::Text("Fans: %d", player.fans);
  ImGui::Text("Money: $%.2f", player.money);
  ImGui::Separator();

  // --- 2. Recording Logic ---
  ImGui::Text("Create New Track");
  static char nameBuffer[128] = "New Song";
  ImGui::InputText("##songname", nameBuffer, IM_ARRAYSIZE(nameBuffer));
  ImGui::SameLine();
  if (ImGui::Button("Rnd Name")) {
    strcpy_s(nameBuffer, GenerateSongName().c_str());
  }
  auto SelectedGenre = BeginDropDownMenu(true);

  // 1. Show the STABLE estimate in the UI
  double uiEstimate = player.GetBaseQuality();
  ImGui::TextColored(ImVec4(1, 1, 0, 1), "Potential Quality: %.1f", uiEstimate);

  if (ImGui::Button("Record Song")) {
    // 2. Roll the ACTUAL unique quality only once when the button is clicked
    double recordedQuality = player.CalcQuality();

    songsMade.emplace_back(std::string(nameBuffer), player.name, 0.99f,
                           recordedQuality, player.fans);

    gameLog.Add("Recorded: " + std::string(nameBuffer) +
                " (Q: " + std::to_string((int)recordedQuality) + ")");
  }

  ImGui::Separator();

  // --- 3. Selection Synchronization ---
  // This ensures our checkboxes always match the vault size
  static std::vector<bool> selectedSongs;
  if (selectedSongs.size() != songsMade.size()) {
    selectedSongs.resize(songsMade.size(), false);
  }

  // --- 4. Album Workshop (Preview Logic) ---
  ImGui::Text("Album Workshop");
  static char albumNameBuffer[128] = "New Album";
  ImGui::InputText("##albumname", albumNameBuffer,
                   IM_ARRAYSIZE(albumNameBuffer));

  std::vector<double> selectedQualities;
  std::vector<int> selectedIndices; // Store indices to delete later

  for (int i = 0; i < (int)songsMade.size(); ++i) {
    if (selectedSongs[i]) {
      selectedQualities.push_back(songsMade[i].quality);
      selectedIndices.push_back(i);
    }
  }

  if (!selectedQualities.empty()) {
    double estAlbumQual = player.CalcAlbumQuality(selectedQualities);
    ImGui::TextColored(ImVec4(0, 1, 1, 1), "Est. Album Quality: %.1f",
                       estAlbumQual);

    if (ImGui::Button("Release Album",
                      ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
      std::vector<Song> albumTracks;
      for (int idx : selectedIndices) {
        albumTracks.push_back(songsMade[idx]);
      }
      auto AlbumGenre = GetAlbumGenre();

      // Construct Album: Name, Artist, Tracks, Quality, Fans
      albumsReleased.emplace_back(
          std::string(albumNameBuffer), // _name
          player.name,                  // _artist (Moved this here)
          std::string(AlbumGenre),      // _genre  (Moved this here)
          albumTracks,                  // _tracks
          estAlbumQual,                 // _quality
          player.fans,                  // _currentFans
          GetRecommendedPrice(estAlbumQual,
                              true) // _price (YOU WERE MISSING THIS!)
      );
      if (albumTracks.size() < 6) {
        gameLog.Add("Released EP: " + std::string(albumNameBuffer));
      } else {
        gameLog.Add("Released LP: " + std::string(albumNameBuffer));
      }

      // CLEANUP: Remove songs and their selection states (Backwards to avoid
      // index shifting)
      for (int i = (int)selectedIndices.size() - 1; i >= 0; --i) {
        int targetIndex = selectedIndices[i];
        songsMade.erase(songsMade.begin() + targetIndex);
        selectedSongs.erase(selectedSongs.begin() + targetIndex);
      }
    }
  } else {
    ImGui::TextDisabled("Select tracks in the vault to form an album...");
  }

  ImGui::Separator();

  // --- 5. The Vault (Display & Interaction) ---
  ImGui::Text("The Vault (%zu songs)", songsMade.size());

  ImGui::BeginChild("VaultScroll", ImVec2(0, 300), true);
  for (size_t i = 0; i < songsMade.size();) {
    ImGui::PushID((int)i);

    bool isSelected = selectedSongs[i];
    if (ImGui::Checkbox("##sel", &isSelected)) {
      selectedSongs[i] = isSelected;
    }

    ImGui::SameLine();
    ImGui::Text("%-20s | Q: %.1f", songsMade[i].name.c_str(),
                songsMade[i].quality);

    ImGui::SameLine(ImGui::GetWindowWidth() - 120);
    if (ImGui::Button("Release Single")) {
      songsReleased.push_back(songsMade[i]);
      gameLog.Add("Released Single: " + songsMade[i].name);

      // Sync erasure
      songsMade.erase(songsMade.begin() + i);
      selectedSongs.erase(selectedSongs.begin() + i);

      ImGui::PopID();
      continue; // Don't increment i
    }

    ImGui::PopID();
    i++;
  }
  ImGui::EndChild();

  ImGui::End();
}

void DrawAnalyticsWindow(const std::vector<Song> &songsReleased,
                         std::vector<Album> &albumsReleased) {
  if (ImGui::Begin("Charts & Analytics")) {
    if (songsReleased.empty()) {
      ImGui::TextDisabled("No releases yet.");
    } else {
      if (ImGui::BeginTable("Charts", 5,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Track");
        ImGui::TableSetupColumn("Hype");
        ImGui::TableSetupColumn("Streams");
        ImGui::TableSetupColumn("Sales");
        ImGui::TableSetupColumn("Rev");
        ImGui::TableHeadersRow();

        for (const auto &song : songsReleased) {
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          ImGui::Text("%s", song.name.c_str());

          ImGui::TableSetColumnIndex(1);
          ImGui::ProgressBar(std::clamp((float)song.hype, 0.0f, 1.0f),
                             ImVec2(-1, 0));

          ImGui::TableSetColumnIndex(2);
          ImGui::Text("%d (+%d)", song.totalStreams, song.dailyStreams);

          ImGui::TableSetColumnIndex(3);
          ImGui::Text("%d", song.totalSales);

          ImGui::TableSetColumnIndex(4);
          ImGui::Text("$%.2f", song.earnings);
        }

        for (const auto &album : albumsReleased) {
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          ImGui::Text("%s", album.name.c_str());

          ImGui::TableSetColumnIndex(1);
          ImGui::ProgressBar(std::clamp((float)album.hype, 0.0f, 1.0f),
                             ImVec2(-1, 0));

          ImGui::TableSetColumnIndex(2);
          ImGui::Text("%d (+%d)", album.totalStreams, album.dailyStreams);

          ImGui::TableSetColumnIndex(3);
          ImGui::Text("%d", album.totalSales);

          ImGui::TableSetColumnIndex(4);
          ImGui::Text("$%.2f", album.earnings);
        }

        ImGui::EndTable();
      }
    }
  }
  ImGui::End();
}

void DrawUpgradeWindow(const char *title, Player &player,
                       std::vector<std::pair<std::string, double>> &items) {

  bool IsSkillWindow = (strcmp(title, "Skills") == 0);

  if (ImGui::Begin(title)) {
    ImGui::Text("Funds: $%.2f", player.money);
    ImGui::Separator();

    for (size_t i = 0; i < items.size(); ++i) {
      auto &item = items[i];
      ImGui::PushID((int)i);

      ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s (Lvl %.1f)",
                         item.first.c_str(), item.second);

      auto UpgradeButton = [&](const char *label, double cost, double gain) {
        bool cannotAfford = (player.money < cost);

        if (cannotAfford)
          ImGui::BeginDisabled();

        if (ImGui::Button(label)) {
          player.money -= cost;
          item.second += gain;
          if (IsSkillWindow) {
            gameLog.Add("Learned " + item.first);
          } else {
            gameLog.Add("Upgraded");
          }
        }

        if (cannotAfford)
          ImGui::EndDisabled();

        ImGui::SameLine();
      };

      // Free study
      if (IsSkillWindow) {
        if (ImGui::Button("Study (Free)")) {
          item.second += 0.1;
        }
      } else {
        UpgradeButton("Minor Upgrade", 10.0, 1.0);
      }
      ImGui::SameLine();
      if (IsSkillWindow) {
        UpgradeButton("Course ($50)", 50.0, 1.0);
        UpgradeButton("Mentor ($250)", 250.0, 2.5);
      } else {
        UpgradeButton("Average Upgrade ($100)", 100.0, 2.0);
        UpgradeButton("Major Upgrade ($500)", 500.0, 3.5);
      }
      ImGui::PopID();
    }

    ImGui::NewLine();
  }
  ImGui::End();
}