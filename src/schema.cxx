#include "schema.hxx"

#include "schema.hxx"

#include <cctype>
#include <sstream>

namespace schema
{
	namespace
	{
		bool isIdentifier(std::string_view key)
		{
			if (key.empty())
				return false;

			unsigned char first = static_cast<unsigned char>(key.front());
			if (!(std::isalpha(first) || key.front() == '_' || key.front() == '$'))
				return false;

			for (char ch : key.substr(1))
			{
				unsigned char c = static_cast<unsigned char>(ch);
				if (!(std::isalnum(c) || ch == '_' || ch == '$'))
					return false;
			}

			return true;
		}

		std::string quotePathKey(std::string_view key)
		{
			std::string out;
			out.reserve(key.size() + 4);
			out += "[\"";

			for (char ch : key)
			{
				switch (ch)
				{
				case '\\':
					out += "\\\\";
					break;
				case '"':
					out += "\\\"";
					break;
				case '\n':
					out += "\\n";
					break;
				case '\r':
					out += "\\r";
					break;
				case '\t':
					out += "\\t";
					break;
				default:
					out += ch;
					break;
				}
			}

			out += "\"]";
			return out;
		}
	}

	ParseContext ParseContext::child(std::string_view key) const
	{
		if (isIdentifier(key))
			return ParseContext{path + "." + std::string(key)};

		return ParseContext{path + quotePathKey(key)};
	}

	ParseContext ParseContext::index(std::size_t index) const
	{
		return ParseContext{path + "[" + std::to_string(index) + "]"};
	}

	namespace detail
	{
		std::string typeName(const Value &value)
		{
			if (value.IsUndefined())
				return "undefined";
			if (value.IsNull())
				return "null";
			if (value.IsBoolean())
				return "boolean";
			if (value.IsNumber())
				return "number";
			if (value.IsBigInt())
				return "bigint";
			if (value.IsString())
				return "string";
			if (value.IsArray())
				return "array";
			if (value.IsFunction())
				return "function";
			if (value.IsObject())
				return "object";
			if (value.IsSymbol())
				return "symbol";

			return "value";
		}

		[[noreturn]] void fail(const Env &env, const ParseContext &ctx, const std::string &message)
		{
			throw TypeError::New(env, ctx.path + " " + message);
		}

		[[noreturn]] void failExpected(const Value &value, const ParseContext &ctx, std::string_view expected)
		{
			throw TypeError::New(value.Env(), ctx.path + " must be " + std::string(expected) + ", got " + typeName(value));
		}

		Object objectFromValue(const Value &value, const ParseContext &ctx, bool coerce, bool plainOnly)
		{
			if (value.IsUndefined() || value.IsNull())
				failExpected(value, ctx, "an object");

			if (plainOnly && (value.IsArray() || value.IsFunction()))
				failExpected(value, ctx, "an object");

			if (coerce)
				return value.ToObject();

			if (!value.IsObject())
				failExpected(value, ctx, "an object");

			return value.As<Object>();
		}

		void validateKnownFields(const Object &object, const ParseContext &ctx, const std::set<std::string> &knownFields)
		{
			Array names = object.GetPropertyNames();

			for (uint32_t i = 0; i < names.Length(); ++i)
			{
				Value key = names.Get(i);
				std::string name = key.ToString().Utf8Value();

				if (!knownFields.contains(name))
					throw TypeError::New(object.Env(), ctx.child(name).path + " is not a recognized field");
			}
		}
	}

	bool BooleanSchema::parseValue(const Value &value, const ParseContext &ctx) const
	{
		if (coerce_)
			return value.ToBoolean().Value();

		if (!value.IsBoolean())
			detail::failExpected(value, ctx, "a boolean");

		return value.As<Boolean>().Value();
	}

	BooleanSchema BooleanSchema::coerce() const
	{
		auto copy = *this;
		copy.coerce_ = true;
		return copy;
	}

	std::string StringSchema::parseValue(const Value &value, const ParseContext &ctx) const
	{
		if (coerce_)
			return value.ToString().Utf8Value();

		if (!value.IsString())
			detail::failExpected(value, ctx, "a string");

		return value.As<String>().Utf8Value();
	}

	StringSchema StringSchema::coerce() const
	{
		auto copy = *this;
		copy.coerce_ = true;
		return copy;
	}

	double NumberSchema::parseValue(const Value &value, const ParseContext &ctx) const
	{
		if (coerce_)
			return value.ToNumber().DoubleValue();

		if (!value.IsNumber())
			detail::failExpected(value, ctx, "a number");

		return value.As<Number>().DoubleValue();
	}

	NumberSchema NumberSchema::coerce() const
	{
		auto copy = *this;
		copy.coerce_ = true;
		return copy;
	}

	std::uint64_t BigIntSchema::parseValue(const Value &value, const ParseContext &ctx) const
	{
		if (!value.IsBigInt())
			detail::failExpected(value, ctx, "a bigint");

		bool lossless = false;
		std::uint64_t result = value.As<BigInt>().Uint64Value(&lossless);

		if (!lossless)
			detail::fail(value.Env(), ctx, "must be a lossless unsigned 64-bit bigint");

		return result;
	}

	Function FunctionSchema::parseValue(const Value &value, const ParseContext &ctx) const
	{
		if (!value.IsFunction())
			detail::failExpected(value, ctx, "a function");

		return value.As<Function>();
	}

	Object NativeObjectSchema::parseValue(const Value &value, const ParseContext &ctx) const
	{
		return detail::objectFromValue(value, ctx, coerce_, plainOnly_);
	}

	NativeObjectSchema NativeObjectSchema::coerce() const
	{
		auto copy = *this;
		copy.coerce_ = true;
		return copy;
	}

	NativeObjectSchema NativeObjectSchema::strict() const
	{
		auto copy = *this;
		copy.plainOnly_ = true;
		return copy;
	}

	NativeObjectSchema NativeObjectSchema::loose() const
	{
		auto copy = *this;
		copy.plainOnly_ = false;
		return copy;
	}

}