#pragma once
#include "AL/Common.hpp"

namespace AL::Collections
{
	template<typename T>
	class Graph
	{
#pragma pack(push, 1)
		struct Node
		{
			T Value;
		};
#pragma pack(pop)

		Node*** nodes; // [y][x][z]

		size_t width;
		size_t height;
		size_t depth;

	public:
		typedef T Type;

		Graph(Graph&& graph)
			: nodes(
				graph.nodes
			),
			width(
				graph.width
			),
			height(
				graph.height
			),
			depth(
				graph.depth
			)
		{
			graph.nodes = nullptr;
			graph.width = 0;
			graph.height = 0;
			graph.depth = 0;
		}

		Graph(const Graph& graph)
			: width(
				graph.width
			),
			height(
				graph.height
			),
			depth(
				graph.depth
			)
		{
			AllocAndCopyNodes(
				graph.nodes,
				nodes,
				graph.width,
				graph.height,
				graph.depth
			);
		}

		Graph(size_t width, size_t height, size_t depth)
			: nodes(
				nullptr
			),
			width(
				width
			),
			height(
				height
			),
			depth(
				depth
			)
		{
			AllocNodes(
				nodes,
				width,
				height,
				depth
			);
		}

		virtual ~Graph()
		{
			FreeNodes(
				nodes,
				GetWidth(),
				GetHeight(),
				GetDepth()
			);
		}

		size_t GetCount() const
		{
			return GetWidth() * GetHeight() * GetDepth();
		}

		size_t GetWidth() const
		{
			return width;
		}

		size_t GetHeight() const
		{
			return height;
		}

		size_t GetDepth() const
		{
			return depth;
		}

		Type& Get(size_t x, size_t y, size_t z)
		{
			return nodes[y][x][z].Value;
		}
		const Type& Get(size_t x, size_t y, size_t z) const
		{
			return nodes[y][x][z].Value;
		}

		Type* Find(size_t x, size_t y, size_t z) const
		{
			return ((x < GetWidth()) && (y < GetHeight()) && (z < GetDepth())) ? &Get(x, y, z) : nullptr;
		}

		bool Contains(size_t x, size_t y, size_t z) const
		{
			return (x < GetWidth()) && (y < GetHeight()) && (z < GetDepth());
		}

		void SetSize(size_t width, size_t height, size_t depth)
		{
			FreeNodes(
				nodes,
				GetWidth(),
				GetHeight(),
				GetDepth()
			);

			AllocNodes(
				nodes,
				width,
				height,
				depth
			);

			this->width = width;
			this->height = height;
			this->depth = depth;
		}

		auto& operator = (Graph&& graph)
		{
			FreeNodes(
				nodes,
				GetWidth(),
				GetHeight(),
				GetDepth()
			);

			nodes = graph.nodes;
			graph.nodes = nullptr;

			width = graph.width;
			graph.width = 0;

			height = graph.height;
			graph.height = 0;

			depth = graph.depth;
			graph.depth = 0;

			return *this;
		}

		auto& operator = (const Graph& graph)
		{
			FreeNodes(
				nodes,
				GetWidth(),
				GetHeight(),
				GetDepth()
			);

			AllocAndCopyNodes(
				graph.nodes,
				nodes,
				graph.width,
				graph.height,
				graph.depth
			);

			width = graph.width;
			height = graph.height;
			depth = graph.depth;

			return *this;
		}

	private:
		static void AllocNodes(Node***& nodes, size_t width, size_t height, size_t depth)
		{
			AL_ASSERT(nodes == nullptr, "Node array already allocated");

			nodes = new Node**[height];

			for (size_t i = 0; i < height; ++i)
			{
				nodes[i] = new Node*[width];

				for (size_t j = 0; j < width; ++j)
				{
					nodes[i][j] = new Node[depth];
				}
			}
		}

		static void FreeNodes(Node***& nodes, size_t width, size_t height, size_t depth)
		{
			if (nodes != nullptr)
			{
				for (size_t i = 0; i < height; ++i)
				{
					for (size_t j = 0; j < width; ++j)
					{
						delete[] nodes[i][j];
					}

					delete[] nodes[i];
				}

				delete[] nodes;
				nodes = nullptr;
			}
		}

		static void AllocAndCopyNodes(Node*** source, Node***& destination, size_t width, size_t height, size_t depth)
		{
			AL_ASSERT(Is_Copy_Assignable<T>::Value, "T is not copy assignable");

			AL_ASSERT(source != nullptr, "Node source array not allocated");
			AL_ASSERT(destination == nullptr, "Node destination array already allocated");

#if defined(AL_DEBUG)
			OS::Debug::WriteLine(
				"Warning: Copying AL::Collections::Graph<%s>",
				Get_Type_Name<T>::Value
			);
#endif

			if constexpr (Is_Copy_Assignable<T>::Value)
			{
				destination = new Node**[height];

				for (size_t i = 0; i < height; ++i)
				{
					destination[i] = new Node*[width];

					for (size_t j = 0; j < width; ++j)
					{
						destination[i][j] = new Node[depth];

						for (size_t t = 0; t < depth; ++t)
						{
							destination[i][j][t] = source[i][j][t];
						}
					}
				}
			}
		}
	};
}
