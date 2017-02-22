#pragma once

namespace mupdfwinrt {
	[Windows::UI::Xaml::Data::Bindable]
	public ref class ContentItem sealed
	{
		private:
			int page;
			Platform::String^ string_orig;
			Platform::String^ string_margin;

		public:
			ContentItem(void);

		property int Page
		{
			int get()
			{
				return (page);
			}

			void set(int value)
			{
				if (value < 0)
					throw ref new Platform::InvalidArgumentException();
				page = value;
			}
		}

		property Platform::String^ StringOrig
		{
			Platform::String^ get()
			{
				return (string_orig);
			}

			void set(Platform::String^ value)
			{
				string_orig = value;
			}
		}

		property Platform::String^ StringMargin
		{
			Platform::String^ get()
			{
				return (string_margin);
			}

			void set(Platform::String^ value)
			{
				string_margin = value;
			}
		}
	};
}
