#pragma once
#include "PipelineItem.h"
#include "PipelineSink.h"

class PipelineFilter : public PipelineItem
{
public:
	PipelineFilter(std::shared_ptr<EntityBlock> blockEntity);
	genericDataBlock FilterData(genericDataBlock& inputData);
	/*{
		genericDataBlock result = _handler.Execute(inputData);
		for(PipelineFilter& filter: _subsequentFilter)
		{
			filter.FilterData(result);
		}
		for(PipelineSink& sink : _sinks)
		{
			sink.ProcessData(result);
		}
	}*/
	void AddSubsequentFilter(PipelineFilter& subsequentFilter);
	void AddSink(PipelineSink& sink);

private:
	//SinkHandler _handler;
	std::list<PipelineFilter> _subsequentFilter;
	std::list<PipelineSink> _sinks;
};