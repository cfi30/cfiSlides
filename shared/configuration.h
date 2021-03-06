/**
 * Copyright (C) 2013  Christian Fillion
 * This file is part of cfiSlides.
 *
 * cfiSlides is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cfiSlides is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cfiSlides.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define DEFAULT_ZOOM           0.5
#define ZOOM_FACTOR            1.25
#define ZOOM_LIMIT             10
#define MOVE_STEPS             2
#define MINIMUM_SIZE           QSize(10, 10)
#define MAXIMUM_THICKNESS      50
#define DEFAULT_SIZE_SCALE     2 // TODO: remove (use movie's size)
#define FILE_FILTER            tr("Diaporama au format cfiSlides (*.csl)")
#define IMAGE_FILTER           tr("Image (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm)")
#define MOVIE_FILTER           tr("Vidéo (*.mpg *.mpeg *.mp4 *.m4v *.ogv *.mkv *.mks *.mov *.qt *.avi *.mng *.mp2 *.wmv *.flv *.m2ts *.mts *.webm)")
#define AUDIO_FILTER           tr("Audio (*.wav *.mp2 *.mp3 *.wma *.ogg *.aif *.acc *.m4a *.mka)")
#define STATUS_TIMEOUT         5000
#define TEST_ICON              "document-new"
#define FALLBACK_THEME         "oxygen"
#define REFRESH_INTERVAL       100
#define PLUGINS_PATH           QCoreApplication::applicationDirPath() + "/plugins/"
#define RECENT_FILES_MAX       6
#define MAX_LOADED_SLIDES      20

#endif // CONFIGURATION_H
