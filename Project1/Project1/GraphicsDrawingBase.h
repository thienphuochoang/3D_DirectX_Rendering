#pragma once
#include "GraphicsDrawing.h"
#include "IndexBuffer.h"

template<class T>
class GraphicsDrawingBase : public GraphicsDrawing
{
protected:
	static bool IsStaticInitialized()
	{
		return !staticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<GraphicsBinding> bind)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic()
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<GraphicsBinding>>& GetStaticBinds() const override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<GraphicsBinding>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<GraphicsBinding>> GraphicsDrawingBase<T>::staticBinds;