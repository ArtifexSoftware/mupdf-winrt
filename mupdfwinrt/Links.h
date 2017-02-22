#pragma once

#include "utils.h"
#include "../shared/status.h"

namespace mupdfwinrt
{
	public ref class Links sealed
	{
	private:
		int type;
		Windows::Foundation::Point upper_left;
		Windows::Foundation::Point lower_right;
		Windows::Foundation::Uri ^uri;
		int page_num;
	public:
		Links(void);

		property int Type
		{
			int get()
			{
				return (type);
			}

			void set(int value)
			{
				if (value > NOT_SET)
					throw ref new Platform::InvalidArgumentException();
				type = value;
			}
		}

		property Windows::Foundation::Point UpperLeft
		{
			Windows::Foundation::Point get()
			{
				return upper_left;
			}

			void set(Windows::Foundation::Point value)
			{
				upper_left = value;
			}
		}

		property Windows::Foundation::Point LowerRight
		{
			Windows::Foundation::Point get()
			{
				return lower_right;
			}

			void set(Windows::Foundation::Point value)
			{
				lower_right = value;
			}
		}

		property int PageNum
		{
			int get()
			{
				return page_num;
			}

			void set(int value)
			{
				page_num = value;
			}
		}

		property Windows::Foundation::Uri^ Uri
		{
			Windows::Foundation::Uri^ get()
			{
				return uri;
			}

			void set(Windows::Foundation::Uri^ value)
			{
				uri = value;
			}
		}
	};
}
