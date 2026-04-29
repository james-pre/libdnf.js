
#pragma once

#include "common.hxx"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

/*
__todo__ parseArgs(std::span<__todo__> &schemas, const Env &env, std::span<const Value> values);

inline __todo__ parseArgs(std::span<__todo__> &schemas, const CallbackInfo &args)
{
	std::vector<Value> values;
	values.reserve(args.Length());

	for (unsigned int i = 0; i < args.Length(); i++)
		values.push_back(args[i]);

	return parseArgs(schemas, args.Env(), values);
}

inline __todo__ parseArgs(std::span<__todo__> &schemas, const Array &array)
{
	std::vector<Value> values;
	values.reserve(array.Length());

	for (unsigned int i = 0; i < array.Length(); i++)
		values.push_back(array[i]);

	return parseArgs(schemas, array.Env(), values);
} */

namespace schema
{

	struct ParseContext
	{
		std::string path = "$";

		ParseContext child(std::string_view key) const;
		ParseContext index(std::size_t index) const;
	};

	namespace detail
	{
		std::string typeName(const Value &value);

		[[noreturn]] void fail(const Env &env, const ParseContext &ctx, const std::string &message);
		[[noreturn]] void failExpected(const Value &value, const ParseContext &ctx, std::string_view expected);

		Object objectFromValue(const Value &value, const ParseContext &ctx, bool coerce, bool plainOnly);
		void validateKnownFields(const Object &object, const ParseContext &ctx, const std::set<std::string> &knownFields);

		template <auto MemberPointer>
		struct MemberPointerTraits;

		template <typename Owner, typename Member, Member Owner::*MemberPointer>
		struct MemberPointerTraits<MemberPointer>
		{
			using owner_type = Owner;
			using member_type = Member;
		};

		template <typename SchemaT>
		struct AcceptsMissing : std::false_type
		{
		};

		template <typename SchemaT>
		concept CoercibleSchema = requires(const SchemaT &schema) {
			{ schema.coerce() };
		};
	}

	template <typename InnerSchema>
	class OptionalSchema;

	template <typename InnerSchema>
	class DefaultSchema;

	template <typename Derived, typename Result>
	class Schema
	{
	public:
		using result_type = Result;

		Result parse(const Value &value) const
		{
			return derived().parseValue(value, ParseContext{path_});
		}

		Result parseAt(const Value &value, const ParseContext &ctx) const
		{
			return derived().parseValue(value, ctx);
		}

		Derived withPath(std::string path) const
		{
			auto copy = derived();
			copy.path_ = std::move(path);
			return copy;
		}

		const std::string &path() const
		{
			return path_;
		}

		OptionalSchema<Derived> optional() const
		{
			return OptionalSchema<Derived>(derived());
		}

		template <typename DefaultValue>
		DefaultSchema<Derived> defaultTo(DefaultValue &&value) const
		{
			return DefaultSchema<Derived>(derived(), Result(std::forward<DefaultValue>(value)));
		}

	protected:
		std::string path_ = "$";

	private:
		const Derived &derived() const
		{
			return static_cast<const Derived &>(*this);
		}
	};

	template <typename Owner>
	class Field
	{
	public:
		using ApplyFunction = std::function<void(Owner &, const Object &, const ParseContext &)>;

		Field(std::string name, ApplyFunction apply) : name_(std::move(name)), apply_(std::move(apply))
		{
		}

		const std::string &name() const
		{
			return name_;
		}

		void apply(Owner &owner, const Object &object, const ParseContext &ctx) const
		{
			apply_(owner, object, ctx);
		}

	private:
		std::string name_;
		ApplyFunction apply_;
	};

	class BooleanSchema : public Schema<BooleanSchema, bool>
	{
	public:
		bool parseValue(const Value &value, const ParseContext &ctx) const;
		BooleanSchema coerce() const;

	private:
		bool coerce_ = false;
	};

	class StringSchema : public Schema<StringSchema, std::string>
	{
	public:
		std::string parseValue(const Value &value, const ParseContext &ctx) const;
		StringSchema coerce() const;

	private:
		bool coerce_ = false;
	};

	class NumberSchema : public Schema<NumberSchema, double>
	{
	public:
		double parseValue(const Value &value, const ParseContext &ctx) const;
		NumberSchema coerce() const;

	private:
		bool coerce_ = false;
	};

	class BigIntSchema : public Schema<BigIntSchema, std::uint64_t>
	{
	public:
		std::uint64_t parseValue(const Value &value, const ParseContext &ctx) const;
	};

	class FunctionSchema : public Schema<FunctionSchema, Function>
	{
	public:
		Function parseValue(const Value &value, const ParseContext &ctx) const;
	};

	class NativeObjectSchema : public Schema<NativeObjectSchema, Object>
	{
	public:
		Object parseValue(const Value &value, const ParseContext &ctx) const;

		NativeObjectSchema coerce() const;
		NativeObjectSchema strict() const;
		NativeObjectSchema loose() const;

	private:
		bool coerce_ = false;
		bool plainOnly_ = true;
	};

	template <typename InnerSchema>
	class OptionalSchema : public Schema<OptionalSchema<InnerSchema>, std::optional<typename InnerSchema::result_type>>
	{
	public:
		using inner_schema_type = InnerSchema;
		using inner_type = typename InnerSchema::result_type;
		using result_type = std::optional<inner_type>;

		explicit OptionalSchema(InnerSchema inner) : inner_(std::move(inner))
		{
		}

		result_type parseValue(const Value &value, const ParseContext &ctx) const
		{
			if (value.IsUndefined() || value.IsNull())
				return std::nullopt;

			return inner_.parseAt(value, ctx);
		}

		auto coerce() const
			requires detail::CoercibleSchema<InnerSchema>
		{
			return OptionalSchema<decltype(inner_.coerce())>(inner_.coerce());
		}

	private:
		InnerSchema inner_;
	};

	template <typename InnerSchema>
	class DefaultSchema : public Schema<DefaultSchema<InnerSchema>, typename InnerSchema::result_type>
	{
	public:
		using inner_schema_type = InnerSchema;
		using result_type = typename InnerSchema::result_type;

		DefaultSchema(InnerSchema inner, result_type fallback) : inner_(std::move(inner)), fallback_(std::move(fallback))
		{
		}

		result_type parseValue(const Value &value, const ParseContext &ctx) const
		{
			if (value.IsUndefined() || value.IsNull())
				return fallback_;

			return inner_.parseAt(value, ctx);
		}

		auto coerce() const
			requires detail::CoercibleSchema<InnerSchema>
		{
			auto coerced = inner_.coerce();
			return DefaultSchema<decltype(coerced)>(std::move(coerced), fallback_);
		}

	private:
		InnerSchema inner_;
		result_type fallback_;
	};

	namespace detail
	{
		template <typename InnerSchema>
		struct AcceptsMissing<OptionalSchema<InnerSchema>> : std::true_type
		{
		};

		template <typename InnerSchema>
		struct AcceptsMissing<DefaultSchema<InnerSchema>> : std::true_type
		{
		};
	}

	template <typename InnerSchema>
	class ArraySchema : public Schema<ArraySchema<InnerSchema>, std::vector<typename InnerSchema::result_type>>
	{
	public:
		using inner_schema_type = InnerSchema;
		using value_type = typename InnerSchema::result_type;
		using result_type = std::vector<value_type>;

		explicit ArraySchema(InnerSchema inner) : inner_(std::move(inner))
		{
		}

		result_type parseValue(const Value &value, const ParseContext &ctx) const
		{
			if (!value.IsArray())
				detail::failExpected(value, ctx, "an array");

			Array array = value.As<Array>();

			result_type result;
			result.reserve(array.Length());

			for (uint32_t i = 0; i < array.Length(); ++i)
				result.push_back(inner_.parseAt(array.Get(i), ctx.index(i)));

			return result;
		}

	private:
		InnerSchema inner_;
	};

	template <typename T>
	class ObjectSchema : public Schema<ObjectSchema<T>, T>
	{
	public:
		using result_type = T;

		ObjectSchema() = default;

		ObjectSchema(std::initializer_list<Field<T>> fields) : fields_(fields)
		{
			for (const auto &field : fields_)
			{
				if (!knownFields_.insert(field.name()).second)
					throw std::invalid_argument("duplicate schema field: " + field.name());
			}
		}

		T parseValue(const Value &value, const ParseContext &ctx) const
		{
			static_assert(std::is_default_constructible_v<T>, "schema::object<T>() requires T to be default-constructible");

			Object object = detail::objectFromValue(value, ctx, coerce_, true);

			if (strict_)
				detail::validateKnownFields(object, ctx, knownFields_);

			T result{};

			for (const auto &field : fields_)
				field.apply(result, object, ctx);

			return result;
		}

		ObjectSchema coerce() const
		{
			auto copy = *this;
			copy.coerce_ = true;
			return copy;
		}

		ObjectSchema strict() const
		{
			auto copy = *this;
			copy.strict_ = true;
			return copy;
		}

		ObjectSchema loose() const
		{
			auto copy = *this;
			copy.strict_ = false;
			return copy;
		}

	private:
		std::vector<Field<T>> fields_;
		std::set<std::string> knownFields_;
		bool coerce_ = false;
		bool strict_ = true;
	};

	class ValueSchema : public Schema<ValueSchema, Value>
	{
	public:
		Value parseValue(const Value &value, const ParseContext &ctx) const
		{
			(void)ctx;
			return value;
		}
	};

	template <auto MemberPointer, typename InnerSchema>
	auto field(std::string name, InnerSchema inner)
	{
		using Traits = detail::MemberPointerTraits<MemberPointer>;
		using Owner = typename Traits::owner_type;
		using Member = typename Traits::member_type;
		using SchemaT = std::decay_t<InnerSchema>;
		using Parsed = typename SchemaT::result_type;

		static_assert(std::is_assignable_v<Member &, Parsed>, "schema::field member type is not assignable from the schema result type");

		auto apply = [name, inner = SchemaT(std::move(inner))](Owner &owner, const Object &object, const ParseContext &ctx)
		{
			ParseContext fieldCtx = ctx.child(name);
			Value value = object.Get(name);

			if (value.IsUndefined() && !detail::AcceptsMissing<SchemaT>::value)
				detail::fail(object.Env(), fieldCtx, "is required");

			owner.*MemberPointer = inner.parseAt(value, fieldCtx);
		};

		return Field<Owner>(std::move(name), std::move(apply));
	}

	inline BooleanSchema boolean()
	{
		return BooleanSchema{};
	}

	inline StringSchema string()
	{
		return StringSchema{};
	}

	inline NumberSchema number()
	{
		return NumberSchema{};
	}

	inline BigIntSchema bigint()
	{
		return BigIntSchema{};
	}

	inline FunctionSchema function()
	{
		return FunctionSchema{};
	}

	inline NativeObjectSchema object()
	{
		return NativeObjectSchema{};
	}

	template <typename T>
	ObjectSchema<T> object(std::initializer_list<Field<T>> fields)
	{
		return ObjectSchema<T>(fields);
	}

	template <typename InnerSchema>
	ArraySchema<std::decay_t<InnerSchema>> array(InnerSchema inner)
	{
		return ArraySchema<std::decay_t<InnerSchema>>(std::move(inner));
	}

	inline ValueSchema js_value()
	{
		return ValueSchema{};
	}

} // namespace schema