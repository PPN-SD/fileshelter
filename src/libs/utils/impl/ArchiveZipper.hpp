/*
 * Copyright (C) 2020 Emeric Poupon
 *
 * This file is part of fileshelter.
 *
 * fileshelter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * fileshelter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fileshelter.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <memory>
#include "utils/IZipper.hpp"

extern "C"
{
	struct archive;
	struct archive_entry;
};

namespace Zip
{
	class ArchiveZipper : public IZipper
	{
		public:
			ArchiveZipper(const EntryContainer& files);
			ArchiveZipper(const ArchiveZipper&) = delete;
			ArchiveZipper& operator=(const ArchiveZipper&) = delete;

		private:
			SizeType writeSome(std::ostream& output) override;
			bool isComplete() const override;
			void abort() override;

			class ArchiveDeleter
			{
				public:
					void operator()(struct ::archive* arch);
			};
			using ArchivePtr = std::unique_ptr<struct ::archive, ArchiveDeleter>;

			class ArchiveEntryDeleter
			{
				public:
					void operator()(struct ::archive_entry* archEntry);
			};
			using ArchiveEntryPtr = std::unique_ptr<struct ::archive_entry, ArchiveEntryDeleter>;

			void prepareCurrentEntry();
			static ArchiveEntryPtr createArchiveEntry(const Entry& entry);
			bool writeSomeCurrentFileData();
			std::int64_t onWriteCallback(const std::byte* buff, std::size_t size);

			const EntryContainer _entries;
			ArchivePtr _archive;

			static inline constexpr std::size_t _writeBlockSize {65536};
			static inline constexpr std::size_t _readBufferSize {65536};
			std::vector<std::byte> _readBuffer;

			EntryContainer::const_iterator _currentEntry;
			ArchiveEntryPtr _currentArchiveEntry;

			std::size_t _currentEntryOffset {};
			std::ostream* _currentOutputStream {};
			std::size_t _bytesWrittenInCurrentOutputStream {};
	};

} // namespace Zip

